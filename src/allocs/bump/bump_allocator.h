//
// Created by Paul on 27/10/2025.
//

#ifndef BUMP_ALLOCATOR_H
#define BUMP_ALLOCATOR_H

#include <memory>
#include <new>
#include <cassert>
#include "platform.h"
#include "numeric"

namespace vallocs::bump {
    template <typename T>
    class bump_allocator {
        // swap this to unique_ptr??
        std::shared_ptr<void> base_ptr_;
        size_t offset_{0};
        size_t last_offset_{0};
        size_t capacity_{0};

        T* allocate_(const size_t n, const size_t alignment = alignof(T)) {
            void* ua_resource = static_cast<char*>(base_ptr_.get()) + offset_;
            size_t free_space = capacity_ >= offset_ ? capacity_ - offset_ : 0;

            void* resource = std::align(alignment, n, ua_resource, free_space);
            if (!resource) return nullptr;

            offset_ += capacity_ - offset_ - free_space + n;
            return static_cast<T*>(resource);
        }

    public:
        explicit bump_allocator(const size_t capacity) noexcept {
            void* base_raw = platform::memory::reserve(capacity);
            if (!base_raw) throw std::bad_alloc();
            if (!platform::memory::commit(base_raw, capacity))
                throw std::bad_alloc();
            base_ptr_ = std::shared_ptr<void>(base_raw, [capacity](void* p) {
                if (p) platform::memory::release(p, capacity);
            });
            capacity_ = capacity;
        }

        explicit bump_allocator(void* buf, const size_t capacity) noexcept {
            base_ptr_ = std::shared_ptr<void>(buf, [](void*) noexcept {
            });
            capacity_ = capacity;
        }

        ~bump_allocator() {
            release();
        }

        T* allocate(const size_t n = 1) {
            if (n > 0 && sizeof(T) > std::numeric_limits<size_t>::max() / n)
                return nullptr;
            return allocate_(sizeof(T) * n, alignof(T));
        }

        std::span<T> allocate_span(std::size_t n) {
            return {allocate(n), n};
        }

        // test this
        T* resize(T* ptr, const std::size_t old_count, const std::size_t new_count) {
            const auto base = static_cast<char*>(base_ptr_.get());
            const auto old_size = old_count * sizeof(T);
            const auto new_size = new_count * sizeof(T);

            if (char* last_begin = base + offset_ - old_size; ptr != reinterpret_cast<T*>(last_begin))
                return nullptr;

            if (new_size <= old_size) {
                offset_ -= old_size - new_size;
                return ptr;
            }

            const std::size_t extra = new_size - old_size;
            if (offset_ + extra > capacity_)
                return nullptr;

            offset_ += extra;
            return ptr;
        }

        void reset() {
            offset_ = 0;
        }

        void release() {
            base_ptr_.reset();
            offset_ = 0;
            capacity_ = 0;
        }

        void rewind (const size_t marker) {
            if (marker <= offset_ && marker <= capacity_) offset_ = marker;
        }

        [[nodiscard]] size_t get_marker() const noexcept {
            return offset_;
        }
    };
}

#endif //BUMP_ALLOCATOR_H
