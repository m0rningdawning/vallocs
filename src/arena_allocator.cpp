//
// Created by Paul on 27/10/2025.
//

#include "arena_allocator.h"

#include "memoryapi.h"

arena::arena_allocator::arena_allocator(size_t capacity) {
    // abstract this later to a cross-platform interface
    void* base_raw = VirtualAlloc(nullptr, capacity, MEM_COMMIT, PAGE_READWRITE);
    if (!base_raw) throw std::bad_alloc();
    base_ptr_ = std::shared_ptr<void>(base_raw, [](void* p) {
        if (p) VirtualFree(p, NULL, MEM_RELEASE);
    });
    capacity_ = capacity;
    offset_ = 0;
}

arena::arena_allocator::arena_allocator(void* buf, size_t capacity) {
    base_ptr_ = std::shared_ptr<void>(buf, [](void*) {});
    capacity_ = capacity;
    offset_ = 0;
}

arena::arena_allocator::~arena_allocator() {
    base_ptr_.reset();
}


void* arena::arena_allocator::allocate(size_t n, size_t allignment) {
}

void arena::arena_allocator::reset() {
}

void arena::arena_allocator::rewind(size_t marker) {
}
