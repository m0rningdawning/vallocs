//
// Created by Pavlo on 04/04/2026.
//

#ifndef VALLOCS_BUDDY_ALLOCATOR_H
#define VALLOCS_BUDDY_ALLOCATOR_H

#include <iostream>
#include "platform.h"

namespace vallocs::buddy {
    template <typename T>
    class buddy_allocator {
        struct buddy_block {
            buddy_block* next;
            bool is_free;
        };

        void* head_ = nullptr;
        void* tail_ = nullptr;
        // static constexpr size_t MAX_LEVELS = 32;
        // buddy_block* free_lists_[MAX_LEVELS]{};
        size_t alignment;
        size_t size_;
        bool owns_memory_ = false;

        void page_(const size_t size) {
            void* base_raw = platform::memory::reserve(size);
            if (!base_raw) throw std::bad_alloc();
            if (!platform::memory::commit(base_raw, size))
                throw std::bad_alloc();
            head_ = base_raw;
            owns_memory_ = true;
        }

        void split_(size_t block_index, size_t level) {
        }

        void merge_(size_t block_index, size_t level) {
        }

        buddy_block* find_best_() const {
        }

    public:
        explicit buddy_allocator(const size_t size) {
        }

        // explicit buddy_allocator(void* buf, const size_t size) {}

        buddy_allocator(const buddy_allocator&) = delete;
        buddy_allocator& operator=(const buddy_allocator&) = delete;

        ~buddy_allocator() {
            if (owns_memory_ && head_) {
#ifdef __linux__
                platform::memory::release(head_, size_);
#endif
#ifdef  _WIN32
                platform::memory::release(head_);
#endif
            }
        }

        [[nodiscard]] T* allocate(size_t n = 1, size_t alignment = 8) {
        }

        void free(T* ptr) {
        }
    };
}

#endif //VALLOCS_BUDDY_ALLOCATOR_H
