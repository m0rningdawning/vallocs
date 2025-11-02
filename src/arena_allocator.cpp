//
// Created by Paul on 27/10/2025.
//

#include "arena_allocator.h"

#include "memoryapi.h"

arena::arena_allocator::arena_allocator(const size_t capacity) {
    // abstract this later to a cross-platform interface
    void* base_raw = VirtualAlloc(nullptr, capacity, MEM_COMMIT, PAGE_READWRITE);
    if (!base_raw) throw std::bad_alloc();
    base_ptr_ = std::shared_ptr<void>(base_raw, [](void* p) {
        if (p) VirtualFree(p, 0, MEM_RELEASE);
    });
    capacity_ = capacity;
    offset_ = 0;
}

arena::arena_allocator::arena_allocator(void* buf, const size_t capacity) {
    base_ptr_ = std::shared_ptr<void>(buf, [](void*) {
    });
    capacity_ = capacity;
    offset_ = 0;
}

arena::arena_allocator::~arena_allocator() {
    base_ptr_.reset();
}

void* arena::arena_allocator::allocate(const size_t n, const size_t alignment) {
    if (offset_ + n <= capacity_) {
        void* ua_resource = static_cast<char*>(base_ptr_.get()) + offset_;
        size_t space = capacity_ >= offset_ ? capacity_ - offset_ : 0;

        void* resource = std::align(alignment, n, ua_resource, space);
        if (!resource) return nullptr;
        offset_ += capacity_ - offset_ - space + n;
        return resource;
    }
    return nullptr;
}

void arena::arena_allocator::reset() {
    base_ptr_.reset();
    offset_ = 0;
    capacity_ = 0;
}

size_t arena::arena_allocator::get_marker() const {
    return offset_;
}

void arena::arena_allocator::rewind(const size_t marker) {
    if (marker <= offset_ && marker <= capacity_) offset_ = marker;
}
