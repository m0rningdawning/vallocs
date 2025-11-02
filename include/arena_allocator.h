//
// Created by Paul on 27/10/2025.
//

#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H
#include <memory>

namespace arena {
    class arena_allocator {
        std::shared_ptr<void> base_ptr_;
        size_t offset_{};
        size_t capacity_{};

    public:
        explicit arena_allocator(size_t capacity);
        explicit arena_allocator(void* buf, size_t capacity);

        ~arena_allocator();

        void* allocate(size_t n, size_t alignment = alignof(std::max_align_t));
        void reset();
        void rewind(size_t marker);
        [[nodiscard]] size_t get_marker() const;
    };
}

#endif //ARENA_ALLOCATOR_H
