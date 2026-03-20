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

namespace vallocs::pool {
    template <typename T>
    class pool_allocator {
        // swap this to unique_ptr??

        T* allocate_(const size_t n, const size_t alignment = alignof(T)) {
        }

    public:
        explicit pool_allocator(const size_t capacity) noexcept {
        }

        explicit pool_allocator(void* buf, const size_t capacity) noexcept {
        }

        ~pool_allocator() {
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

        T* resize(T* ptr, const std::size_t old_count, const std::size_t new_count) {
        }

        void reset() {
        }

        void release() {
        }

        void rewind (const size_t marker) {
        }

        [[nodiscard]] size_t get_marker() const noexcept {
            return offset_;
        }
    };
}

#endif //BUMP_ALLOCATOR_H
