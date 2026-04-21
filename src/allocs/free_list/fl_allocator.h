//
// Created by Paul on 27/10/2025.
//

#ifndef FL_ALLOCATOR_H
#define FL_ALLOCATOR_H

#include <iostream>
#include <cassert>
#include "platform.h"

namespace vallocs::fl {
    enum class policy {
        FIND_FIRST = 1,
        FIND_BEST = 2,
    };

    template <typename T>
    class free_list {
        struct fl_header {
            size_t block_size;
            size_t padding;
        };

        struct fl_chunk {
            fl_chunk* next;
            mutable size_t size;
        };

        void* base_ptr_ = nullptr;
        size_t size_;
        size_t used_;
        fl_chunk* head_;
        policy policy_;

        void page_(const size_t size) {
            void* base_raw = platform::memory::reserve(size);
            if (!base_raw) throw std::bad_alloc();
            if (!platform::memory::commit(base_raw, size)) throw std::bad_alloc();
            base_ptr_ = base_raw;
        }

        void node_insert_(fl_chunk* prev_node, fl_chunk* new_node) {
            if (prev_node == nullptr) {
                new_node->next = head_;
                head_ = new_node;
            }
            else {
                new_node->next = prev_node->next;
                prev_node->next = new_node;
            }
        }

        void node_remove_(fl_chunk* prev_node, const fl_chunk* del_node) {
            if (prev_node == nullptr) {
                head_ = del_node->next;
            }
            else {
                prev_node->next = del_node->next;
            }
        }

        static size_t calc_padding_(const uintptr_t ptr, const uintptr_t alignment, const size_t header_size) {
            uintptr_t p = ptr + header_size;
            uintptr_t padding = 0;
            if (p % alignment != 0) {
                padding = alignment - (p % alignment);
            }
            return padding + header_size;
        }

        fl_chunk* find_first_(const size_t size, const size_t alignment, size_t* padding_,
                              fl_chunk** prev_node_) const {
            fl_chunk* node = head_;
            fl_chunk* prev_node = nullptr;
            size_t padding = 0;

            while (node != nullptr) {
                padding = calc_padding_(reinterpret_cast<uintptr_t>(node), static_cast<uintptr_t>(alignment),
                                        sizeof(fl_header));
                if (node->size >= size + padding) {
                    break;
                }
                prev_node = node;
                node = node->next;
            }

            if (padding_) *padding_ = padding;
            if (prev_node_) *prev_node_ = prev_node;
            return node;
        }

        fl_chunk* find_best_(const size_t size, const size_t alignment, size_t* padding_, fl_chunk** prev_node_) const {
            size_t smallest_diff = ~static_cast<size_t>(0);
            fl_chunk* node = head_;
            fl_chunk* prev_node = nullptr;
            fl_chunk* best_node = nullptr;
            fl_chunk* best_prev_node = nullptr;
            size_t padding = 0;
            size_t best_padding = 0;

            while (node != nullptr) {
                padding = calc_padding_(reinterpret_cast<uintptr_t>(node), static_cast<uintptr_t>(alignment),
                                        sizeof(fl_header));
                if (const size_t required_space = size + padding;
                    node->size >= required_space && (node->size - required_space < smallest_diff)) {
                    best_node = node;
                    best_prev_node = prev_node;
                    best_padding = padding;
                    smallest_diff = node->size - required_space;
                }
                prev_node = node;
                node = node->next;
            }
            if (padding_) *padding_ = best_padding;
            if (prev_node_) *prev_node_ = best_prev_node;
            return best_node;
        }

        void coalescence_(fl_chunk* prev_node, fl_chunk* free_node) {
            if (free_node->next != nullptr &&
                reinterpret_cast<char*>(free_node) + free_node->size == reinterpret_cast<char*>(free_node->next)) {
                free_node->size += free_node->next->size;
                node_remove_(free_node, free_node->next);
            }

            if (prev_node != nullptr &&
                reinterpret_cast<char*>(prev_node) + prev_node->size == reinterpret_cast<char*>(free_node)) {
                prev_node->size += free_node->size;
                node_remove_(prev_node, free_node);
            }
        }

    public:
        explicit free_list(const size_t size, const policy policy) {
            page_(size);
            size_ = size;
            used_ = 0;
            head_ = static_cast<fl_chunk*>(base_ptr_);
            head_->next = nullptr;
            head_->size = size;
            policy_ = policy;
        }

        free_list(const free_list&) = delete;
        free_list& operator=(const free_list&) = delete;

        ~free_list() {
#ifdef __linux__
            platform::memory::release(base_ptr_, size_);
#endif
#ifdef _WIN32
            platform::memory::release(base_ptr_);
#endif
        }

        [[nodiscard]] T* allocate(size_t size, size_t alignment) {
            fl_chunk* prev_node = nullptr;
            fl_chunk* node = nullptr;
            size_t padding = 0;

            if (size < sizeof(fl_chunk)) {
                size = sizeof(fl_chunk);
            }
            if (alignment < 8) {
                alignment = 8;
            }

            if (policy_ == policy::FIND_BEST) {
                node = find_best_(size, alignment, &padding, &prev_node);
            }
            else {
                node = find_first_(size, alignment, &padding, &prev_node);
            }
            if (node == nullptr) {
                assert(0 && "Free list has no free memory!");
            }

            const size_t alignment_padding = padding - sizeof(fl_header);
            const size_t required_space = size + padding;

            if (const size_t remaining = node->size - required_space; remaining >= sizeof(fl_chunk)) {
                auto* new_node = reinterpret_cast<fl_chunk*>(reinterpret_cast<char*>(node) + required_space);
                new_node->size = remaining;
                node_insert_(node, new_node);
                node->size = required_space;
            }

            node_remove_(prev_node, node);
            auto* header_ptr = reinterpret_cast<fl_header*>(reinterpret_cast<char*>(node) + alignment_padding);
            header_ptr->block_size = required_space;
            header_ptr->padding = alignment_padding;
            used_ += required_space;

            return reinterpret_cast<T*>(reinterpret_cast<char*>(header_ptr) + sizeof(fl_header));
        }

        void free(T* ptr) {
            fl_chunk* prev_node = nullptr;

            if (ptr == nullptr) {
                return;
            }

            auto* header = reinterpret_cast<fl_header*>(static_cast<char*>(ptr) - sizeof(fl_header));
            auto* free_node = reinterpret_cast<fl_chunk*>(header);
            free_node->size = header->block_size + header->padding;
            free_node->next = nullptr;
            fl_chunk* node = head_;

            while (node != nullptr) {
                if (reinterpret_cast<fl_chunk*>(ptr) < node) {
                    node_insert_(prev_node, free_node);
                    break;
                }
                prev_node = node;
                node = node->next;
            }

            if (node == nullptr) {
                node_insert_(prev_node, free_node);
            }
            used_ -= free_node->size;
            coalescence_(prev_node, free_node);
        }
    };
}

#endif // FL_ALLOCATOR_H
