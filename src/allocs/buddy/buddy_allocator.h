//
// Created by Pavlo on 04/04/2026.
//

#ifndef VALLOCS_BUDDY_ALLOCATOR_H
#define VALLOCS_BUDDY_ALLOCATOR_H

#include <cassert>
#include <iostream>
#include "platform.h"

namespace vallocs::buddy {
    constexpr bool is_power_of_two(size_t x) { return x && ((x & (x - 1)) == 0); }

    constexpr size_t align_forward_size(size_t size, size_t alignment) {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    template <typename T>
    class buddy_allocator {
        struct buddy_block {
            size_t size;
            bool is_free;
        };

        buddy_block* head_ = nullptr;
        buddy_block* tail_ = nullptr;
        size_t alignment_;
        size_t size_;
        bool owns_memory_ = false;

        void page_(const size_t size) {
            void* base_raw = platform::memory::reserve(size);
            if (!base_raw) throw std::bad_alloc();
            if (!platform::memory::commit(base_raw, size)) throw std::bad_alloc();
            head_ = static_cast<buddy_block*>(base_raw);
            owns_memory_ = true;
        }

        static buddy_block* bb_next_(buddy_block* block) {
            return reinterpret_cast<buddy_block*>(reinterpret_cast<char*>(block) + block->size);
        }

        buddy_block* split_(buddy_block* block, size_t size) {
            if (block && size > 0) {
                while (block->size > size) {
                    size_t sh = block->size >> 1;
                    block->size = sh;
                    block->is_free = true;

                    buddy_block* buddy = bb_next_(block);
                    buddy->size = sh;
                    buddy->is_free = true;
                }
                if (size <= block->size) {
                    return block;
                }
            }
            return nullptr;
        }

        [[nodiscard]] size_t size_required_(size_t requested_size) const {
            size_t actual_size = alignment_;
            requested_size += sizeof(buddy_block);
            requested_size = align_forward_size(requested_size, alignment_);

            while (requested_size > actual_size) {
                actual_size <<= 1;
            }
            return actual_size;
        }

        void merge_() {
            bool merged;
            do {
                merged = false;
                buddy_block* curr = head_;
                while (curr < tail_) {
                    buddy_block* next = bb_next_(curr);
                    if (next >= tail_) break;

                    if (curr->is_free && next->is_free && curr->size == next->size) {
                        // Ensure they are true buddies
                        size_t offset = reinterpret_cast<char*>(curr) - reinterpret_cast<char*>(head_);
                        if (offset % (curr->size << 1) == 0) {
                            curr->size <<= 1;
                            merged = true;
                            continue;
                        }
                    }
                    curr = bb_next_(curr);
                }
            }
            while (merged);
        }

        buddy_block* find_best_(buddy_block* head, buddy_block* tail, size_t size) {
            buddy_block* best = nullptr;
            buddy_block* curr = head;

            while (curr < tail) {
                if (curr->is_free && curr->size >= size) {
                    if (best == nullptr || curr->size < best->size) {
                        best = curr;
                    }
                }
                curr = bb_next_(curr);
            }

            if (best != nullptr) {
                return split_(best, size);
            }
            return nullptr;
        }

        void init_(void* data, size_t size, size_t alignment) {
            assert(data != nullptr);
            assert(is_power_of_two(size) && "size is not a power-of-two");
            assert(is_power_of_two(alignment) && "alignment is not a power-of-two");

            if (alignment < sizeof(buddy_block)) {
                size_t min_align = 1;
                while (min_align < sizeof(buddy_block)) min_align <<= 1;
                alignment = min_align;
            }

            assert(reinterpret_cast<uintptr_t>(data) % alignment == 0 && "data is not aligned to minimum alignment");

            head_ = static_cast<buddy_block*>(data);
            head_->size = size;
            head_->is_free = true;

            tail_ = bb_next_(head_);
            alignment_ = alignment;
            size_ = size;
        }

    public:
        explicit buddy_allocator(const size_t size, const size_t alignment = sizeof(buddy_block)) :
            alignment_(alignment), size_(size) {
            page_(size);
            init_(head_, size, alignment);
        }

        explicit buddy_allocator(void* buf, const size_t size, const size_t alignment = sizeof(buddy_block)) :
            alignment_(alignment), size_(size) {
            init_(buf, size, alignment);
        }

        buddy_allocator(const buddy_allocator&) = delete;
        buddy_allocator& operator=(const buddy_allocator&) = delete;

        ~buddy_allocator() {
            if (owns_memory_ && head_) {
#ifdef __linux__
                platform::memory::release(head_, size_);
#endif
#ifdef _WIN32
                platform::memory::release(head_);
#endif
            }
        }

        [[nodiscard]] T* allocate(size_t n = 1, size_t alignment = 8) {
            size_t total_size = n * sizeof(T);
            if (total_size == 0) return nullptr;

            size_t actual_size = size_required_(total_size);

            buddy_block* found = find_best_(head_, tail_, actual_size);
            if (found == nullptr) {
                merge_();
                found = find_best_(head_, tail_, actual_size);
            }

            if (found != nullptr) {
                found->is_free = false;
                return reinterpret_cast<T*>(reinterpret_cast<char*>(found) + alignment_);
            }

            throw std::bad_alloc();
        }

        void free(T* ptr) {
            if (ptr != nullptr) {
                assert(reinterpret_cast<buddy_block*>(ptr) > head_);
                assert(reinterpret_cast<buddy_block*>(ptr) < tail_);

                auto* block = reinterpret_cast<buddy_block*>(reinterpret_cast<char*>(ptr) - alignment_);
                block->is_free = true;
            }
        }
    };
} // namespace vallocs::buddy

#endif // VALLOCS_BUDDY_ALLOCATOR_H
