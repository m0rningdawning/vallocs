//
// Created by Paul on 27/10/2025.
//

#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <algorithm>
#include <cstdint>
#include <iostream>
#include "platform.h"

namespace vallocs::pool {
    template <typename T>
    class pool_allocator {
        struct fl_chunk {
            fl_chunk* next;
        };

        void* base_ptr_ = nullptr;
        size_t size_;
        size_t chunk_size_;
        size_t n_chunks_;
        fl_chunk* head_;
        bool owns_memory_ = false;

        void page_(const size_t size) {
            void* base_raw = platform::memory::reserve(size);
            if (!base_raw)
                throw std::bad_alloc();
            if (!platform::memory::commit(base_raw, size))
                throw std::bad_alloc();
            base_ptr_ = base_raw;
            owns_memory_ = true;
        }

        static constexpr size_t min_chunk_size_() {
            return (sizeof(fl_chunk) > sizeof(T)) ? sizeof(fl_chunk) : sizeof(T);
        }

    public:
        explicit pool_allocator(const size_t size) {
            page_(size);
            size_ = size;
            n_chunks_ = 4;
            chunk_size_ = std::max(size_ / n_chunks_, min_chunk_size_());
            n_chunks_ = size_ / chunk_size_;
            free_all();
        }

        explicit pool_allocator(const size_t size, const size_t chunk_size) {
            if (size == 0 || chunk_size == 0) {
                throw std::invalid_argument("Arguments [size, chunk_size] cannot be 0!");
            }
            if (chunk_size > size) {
                std::cerr << "Chunk can't be larger than the overall size! Will shrink to fit.\n";
                chunk_size_ = size;
            }
            else {
                chunk_size_ = chunk_size;
            }
            chunk_size_ = std::max(chunk_size_, min_chunk_size_());
            size_ = size;
            n_chunks_ = size_ / chunk_size_;
            page_(size);
            free_all();
        }

        // explicit pool_allocator(const size_t size, const size_t chunk_size, const size_t n_chunks) noexcept {
        //     try {
        //         page_(size);
        //     } catch (std::runtime_error(&e)) {
        //         std::cout << e.what() << "\n";
        //         exit(-1);
        //     }
        //     size_ = size;
        //     n_chunks_ = n_chunks;
        //     chunk_size_ = chunk_size;
        // }

        explicit pool_allocator(void* buf, const size_t size) {
            base_ptr_ = buf;
            size_ = size;
            n_chunks_ = 4;
            chunk_size_ = std::max(size_ / n_chunks_, min_chunk_size_());
            n_chunks_ = size_ / chunk_size_;
            free_all();
        }

        pool_allocator(const pool_allocator&) = delete;
        pool_allocator& operator=(const pool_allocator&) = delete;

        ~pool_allocator() {
            if (owns_memory_ && base_ptr_) {
#ifdef __linux__
                platform::memory::release(base_ptr_, size_);
#endif
#ifdef  _WIN32
                platform::memory::release(base_ptr_);
#endif
            }
        }

        [[nodiscard]] T* allocate() {
            fl_chunk* chunk = head_;
            if (chunk == nullptr) {
                std::cerr << "Pool allocator has no free memory\n";
                return nullptr;
            }
            head_ = head_->next;
            return reinterpret_cast<T*>(chunk);
        }

        void free(T* ptr) {
            if (!ptr) return;
            auto* p = reinterpret_cast<uint8_t*>(ptr);
            auto* start = static_cast<uint8_t*>(base_ptr_);
            auto* end = start + size_;
            if (p >= start && p < end) {
                auto* chunk = reinterpret_cast<fl_chunk*>(ptr);
                chunk->next = head_;
                head_ = chunk;
            }
        }

        void free_all() {
            head_ = nullptr;
            auto* byte_ptr = static_cast<uint8_t*>(base_ptr_);
            for (size_t i = 0; i < n_chunks_; ++i) {
                auto* ptr = byte_ptr + (i * chunk_size_);
                auto* chunk = reinterpret_cast<fl_chunk*>(ptr);
                chunk->next = head_;
                head_ = chunk;
            }
        }
    };
}

#endif // POOL_ALLOCATOR_H
