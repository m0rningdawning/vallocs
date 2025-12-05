//
// Created by Paul on 22/11/2025.
//

#ifndef VALLOCS_STACK_ALLOCATOR_H
#define VALLOCS_STACK_ALLOCATOR_H

#include <memory>
#include <cstdint>
#include <cstddef>
#include <new>
#include <limits>
#include <cassert>
#include "platform.h"

// Test that shi
namespace vallocs::stack {
    struct alloc_header {
        std::size_t previous_offset{0};
        std::size_t size{0};
        std::uint16_t padding{0};
    };

    struct stack_header {
        std::size_t capacity{0};
        std::size_t offset{0};
        std::uint16_t alignment{alignof(std::max_align_t)};
        std::uint32_t allocation_count{0};

        void init(const std::size_t cap, const std::size_t align = alignof(std::max_align_t)) noexcept {
            alignment = static_cast<std::uint16_t>(align);
            capacity = cap;
            offset = 0;
            allocation_count = 0;
        }

        std::size_t used() const noexcept { return offset; }
        std::size_t remaining() const noexcept { return capacity - used(); }
    };

    inline stack_header* get_stack_header(void* base_usable) noexcept {
        if (!base_usable) return nullptr;
        auto* p = static_cast<std::byte*>(base_usable);
        return reinterpret_cast<stack_header*>(p) - 1;
    }

    inline const stack_header* get_stack_header(const void* base_usable) noexcept {
        if (!base_usable) return nullptr;
        auto* p = static_cast<const std::byte*>(base_usable);
        return reinterpret_cast<const stack_header*>(p) - 1;
    }

    template <typename T>
    class stack_allocator {
        std::shared_ptr<void> base_ptr_;
        std::size_t capacity_{0};
        std::size_t offset_{0};

    public:
        template <typename U>
        struct rebind {
            using other = stack_allocator<U>;
        };

        stack_allocator() noexcept = default;

        stack_allocator(void* base_ptr, std::size_t capacity) noexcept
            : base_ptr_(base_ptr, [](void*) noexcept {
              }),
              capacity_(capacity) {
            if (auto* hdr = get_stack_header(base_ptr_.get())) {
                // use max_align_t as default alignment for the region itself
                hdr->init(capacity_, alignof(std::max_align_t));
                offset_ = hdr->offset;
            }
        }

        ~stack_allocator() {
            clear();
        };

        template <typename U>
        explicit stack_allocator(const stack_allocator<U>& other) noexcept
            : base_ptr_(other.base_ptr_),
              capacity_(other.capacity_),
              offset_(other.offset_) {
        }

        // especially this!!
        explicit stack_allocator(const std::size_t capacity) {
            if (capacity == 0) throw std::bad_alloc();

            constexpr std::size_t header_bytes = sizeof(stack_header);
            const std::size_t total_bytes = header_bytes + capacity;
            capacity_ = capacity;

            void* region = platform::bump::platform_memory::reserve(total_bytes);
            if (!region) throw std::bad_alloc();
            if (!platform::bump::platform_memory::commit(region, total_bytes)) {
                platform::bump::platform_memory::release(region, total_bytes);
                throw std::bad_alloc();
            }

            auto* region_bytes = static_cast<std::byte*>(region);
            void* base_usable = region_bytes + header_bytes;

            base_ptr_ = std::shared_ptr<void>(base_usable, [total_bytes](void* p) {
                if (!p) return;
                auto* usable = static_cast<std::byte*>(p);
                auto* region_start = usable - sizeof(stack_header);
                platform::bump::platform_memory::release(region_start, total_bytes);
            });

            if (auto* hdr = get_stack_header(base_ptr_.get())) {
                hdr->init(capacity_, alignof(std::max_align_t));
                offset_ = hdr->offset;
            }
            else {
                throw std::bad_alloc();
            }
        }

        [[nodiscard]] T* allocate(std::size_t n) {
            if (n == 0) return nullptr;

            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
                throw std::bad_alloc();

            void* base = base_ptr_.get();
            if (!base) throw std::bad_alloc();

            auto* hdr = get_stack_header(base);
            if (!hdr) throw std::bad_alloc();

            const std::size_t total_size = n * sizeof(T);
            const std::size_t align = std::max<std::size_t>(alignof(T), hdr->alignment);

            auto* region_start = static_cast<std::byte*>(base);
            auto* current_top = region_start + hdr->offset;

            const std::size_t free_space = (hdr->offset <= hdr->capacity)
                                               ? hdr->capacity - hdr->offset
                                               : 0;
            // need space for header + payload (plus alignment padding)
            if (free_space < sizeof(alloc_header) + total_size)
                throw std::bad_alloc();

            // First, reserve space for alloc_header; start tentative user area after it.
            std::byte* header_pos = current_top;

            void* user_ua = header_pos + sizeof(alloc_header);
            std::size_t ava = free_space - sizeof(alloc_header);

            void* user_ptr = user_ua;
            if (std::align(align, total_size, user_ptr, ava) == nullptr)
                throw std::bad_alloc();

            auto* user_byte = static_cast<std::byte*>(user_ptr);

            // padding from end of header area to aligned payload start
            const std::size_t padding =
                static_cast<std::size_t>(user_byte - (header_pos + sizeof(alloc_header)));

            // Place alloc_header immediately before user pointer
            auto* ah = reinterpret_cast<alloc_header*>(user_byte - sizeof(alloc_header));
            ah->previous_offset = hdr->offset;
            ah->size = total_size;
            ah->padding = static_cast<std::uint16_t>(padding);

            // New top at end of payload
            const std::byte* new_top = user_byte + total_size;
            hdr->offset = static_cast<std::size_t>(new_top - region_start);
            hdr->allocation_count += 1;

            offset_ = hdr->offset;

            return reinterpret_cast<T*>(user_byte);
        }

        void deallocate(T* p, std::size_t /* n */) {
            if (!p || !base_ptr_) return;

            void* base = base_ptr_.get();
            auto* hdr = get_stack_header(base);
            if (!hdr) return;

            const auto* region_start = static_cast<std::byte*>(base);
            auto* user_byte = reinterpret_cast<std::byte*>(p);

            const auto* ah = reinterpret_cast<alloc_header*>(user_byte - sizeof(alloc_header));

            // expected current offset if this is the top block
            const std::size_t expected_offset =
                static_cast<std::size_t>((user_byte + ah->size) - region_start);

            // LIFO check
            if (hdr->offset == expected_offset) {
                hdr->offset = ah->previous_offset;
                if (hdr->allocation_count > 0)
                    --hdr->allocation_count;
                offset_ = hdr->offset;
            }
            else {
                // out-of-order free; ignore or assert in debug
                assert(false && "stack_allocator::deallocate out of LIFO order");
            }
        }

        void* base_ptr() const noexcept {
            return base_ptr_.get();
        }

        std::size_t capacity() const noexcept {
            return capacity_;
        }

        std::size_t offset() const noexcept {
            return offset_;
        }

        void clear() {
            if (!base_ptr_) return;
            auto* hdr = get_stack_header(base_ptr_.get());
            if (!hdr) return;
            hdr->offset = 0;
            hdr->allocation_count = 0;
            offset_ = 0;
        }

        // template <typename U>
        // friend class stack_allocator;
        //
        // template <typename T1, typename U1>
        // friend bool operator==(const stack_allocator<T1>&,
        //                        const stack_allocator<U1>&) noexcept;
        //
        // template <typename T1, typename U1>
        // friend bool operator!=(const stack_allocator<T1>&,
        //                        const stack_allocator<U1>&) noexcept;
    };

    // template <typename T, typename U>
    // bool operator==(const stack_allocator<T>& lhs,
    //                 const stack_allocator<U>& rhs) noexcept {
    //     return lhs.base_ptr_ == rhs.base_ptr_;
    // }
    //
    // template <typename T, typename U>
    // bool operator!=(const stack_allocator<T>& lhs,
    //                 const stack_allocator<U>& rhs) noexcept {
    //     return !(lhs == rhs);
    // }
} // namespace vallocs::stack

#endif //VALLOCS_STACK_ALLOCATOR_H
