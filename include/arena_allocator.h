//
// Created by Paul on 27/10/2025.
//

#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <memory>
#include <new>
#include <cassert>
#include <windows.h>

namespace arena {
    template <typename T>
    class arena_allocator {
        std::shared_ptr<void> base_ptr_;
        size_t offset_{0};
        size_t capacity_{0};

        T* allocate_(const size_t n, const size_t alignment = alignof(T)) {
            void* ua_resource = static_cast<char*>(base_ptr_.get()) + offset_;
            size_t space = capacity_ >= offset_ ? capacity_ - offset_ : 0;

            void* resource = std::align(alignment, n, ua_resource, space);
            if (!resource) return nullptr;
            offset_ += capacity_ - offset_ - space + n;
            return static_cast<T*>(resource);
        }

    public:
        explicit arena_allocator(const size_t capacity) {
            void* base_raw = VirtualAlloc(nullptr, capacity, MEM_COMMIT, PAGE_READWRITE);
            if (!base_raw) throw std::bad_alloc();
            base_ptr_ = std::shared_ptr<void>(base_raw, [](void* p) {
                if (p) VirtualFree(p, 0, MEM_RELEASE);
            });
            capacity_ = capacity;
        }

        explicit arena_allocator(void* buf, const size_t capacity) {
            base_ptr_ = std::shared_ptr<void>(buf, [](void*) {});
            capacity_ = capacity;
        }

        ~arena_allocator() {
            base_ptr_.reset();
        }

        T* allocate(const size_t n = 1) {
            return allocate_(sizeof(T) * n, alignof(T));
        }

        void reset() {
            base_ptr_.reset();
            offset_ = 0;
            capacity_ = 0;
        }

        [[nodiscard]] size_t get_marker() const {
            return offset_;
        }

        void rewind(const size_t marker) {
            if (marker <= offset_ && marker <= capacity_) offset_ = marker;
        }
    };
}

#endif //ARENA_ALLOCATOR_H