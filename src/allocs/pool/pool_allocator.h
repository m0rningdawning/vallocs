//
// Created by Paul on 27/10/2025.
//

#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <memory>
#include <iostream>
#include "platform.h"

namespace vallocs::pool {
    template <typename T>
    class pool_allocator {
        struct fl_node {
            fl_node* next;
        };

        void* base_ptr_;
        size_t size_;
        size_t chunk_size_;
        size_t n_chunks_;
        fl_node* head = nullptr;
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
        explicit pool_allocator(const size_t size) {
            try {
                page_(size);
            }
            catch (std::runtime_error (&e)) {
                std::cout << e.what() << "\n";
                exit(-1);
            }
            size_ = size;
            n_chunks_ = 5; // 5 for now to play around
            chunk_size_ = size_ / n_chunks_;
            free_all();
        }

        explicit pool_allocator(const size_t size, const size_t chunk_size) {
            try {
                page_(size);
            }
            catch (std::runtime_error (&e)) {
                std::cout << e.what() << "\n";
                exit(-1);
            }
            if (chunk_size > size) {
                std::cout << "Chunk can't be larger than the overall size! Will shrink to fit.\n";
                chunk_size_ = size;
            }
            else {
                chunk_size_ = chunk_size;
            }
            size_ = size;
            n_chunks_ = size_ / chunk_size_;
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
            n_chunks_ = 5;
            chunk_size_ = size_ / n_chunks_;
            free_all();
        }

        ~pool_allocator() {
            if (owns_memory_ && base_ptr_) {
                platform::memory::release(base_ptr_, size_);
            }
        }

        T* allocate() {
            fl_node *node = head;
	        if (node == nullptr) {
	            std::cout << "Pool allocator has no free memory\n";
		        return nullptr;
	        }
	        head = head->next;
	        return static_cast<T*>(static_cast<void*>(node));
        }

        // std::span<T> allocate_span(std::size_t n) {
        //     return {allocate(n), n};
        // }

        void free() {
        }

        void free_all() {
            auto* byte_ptr = static_cast<uint8_t*>(base_ptr_);
            for (size_t i = 0; i < n_chunks_; ++i) {
                auto* ptr = byte_ptr + (i * chunk_size_);
                auto* node = static_cast<fl_node*>(ptr);
                node->next = head;
                head = node;
            }
        }
    };
}

#endif //POOL_ALLOCATOR_H
