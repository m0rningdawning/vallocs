//
// Created by Paul on 27/10/2025.
//

#ifndef FL_ALLOCATOR_H
#define FL_ALLOCATOR_H

#include <algorithm>
#include <cstdint>
#include <memory>
#include <iostream>
#include "platform.h"

namespace vallocs::fl {
    enum class policy {
        FIND_FIRST = 1,
        FIND_BEST = 2,
    };

    struct fl_header {
        size_t block_size;
        size_t padding;
    };

    struct fl_chunk {
        fl_chunk* next;
        size_t size;
    };

    template <typename T>
    class free_list{
        void* base_ptr_;
        size_t size_;
        size_t used_;
        fl_chunk* head_;
        policy policy_;
        bool owns_memory_ = false;

        void page_(const size_t size) {
            void* base_raw = platform::memory::reserve(size);
            if (!base_raw) throw std::bad_alloc();
            if (!platform::memory::commit(base_raw, size))
                throw std::bad_alloc();
            base_ptr_ = base_raw;
            owns_memory_ = true;
        }

    public:
        explicit free_list(const size_t size) {}

        ~free_list() {}

        free_list(const free_list&) = delete;
        free_list& operator=(const free_list&) = delete;

        fl_chunk find_first() {}
        fl_chunk find_best() {}

        [[nodiscard]] T* allocate() {}
        void free(T* ptr) {}
        void coalescence () {}

    };
}

#endif //POOL_ALLOCATOR_H
