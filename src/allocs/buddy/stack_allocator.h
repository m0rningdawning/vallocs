//
// Created by Paul on 22/11/2025.
//

#ifndef VALLOCS_STACK_ALLOCATOR_H
#define VALLOCS_STACK_ALLOCATOR_H
#include <memory>

namespace vallocs::stack {
    struct stack_header {
        uint8_t padding;
    };

    template <typename T>
    class stack_allocator {
        std::shared_ptr<void> base_ptr_;
        size_t capacity_{0};
        size_t offset_{0};

    public:
        // Rebind structure to obtain an allocator for a different type.
        template <typename U>
        struct rebind {
            using other = stack_allocator<U>;
        };

        // Default constructor
        stack_allocator() noexcept;

        // Constructor to initialize with a memory block.
        stack_allocator(void* base_ptr, std::size_t capacity) noexcept;

        // Copy constructor
        template <typename U>
        stack_allocator(const stack_allocator<U>& other) noexcept;

        // Destructor
        ~stack_allocator() = default;

        // Allocate memory for n objects of type T.
        [[nodiscard]] T* allocate(std::size_t n);

        // Deallocate memory. In a simple stack allocator, this might only
        // deallocate if it's the most recent allocation.
        void deallocate(T* p, std::size_t n);

        // Returns the underlying memory block.
        void* base_ptr() const noexcept;

        // Returns the total capacity of the allocator.
        std::size_t capacity() const noexcept;

        // Returns the currently used memory.
        std::size_t used_memory() const noexcept;

        // Clears all allocations.
        void clear();
    };

    // Comparison operators
    template <typename T, typename U>
    bool operator==(const stack_allocator<T>& lhs, const stack_allocator<U>& rhs) noexcept;

    template <typename T, typename U>
    bool operator!=(const stack_allocator<T>& lhs, const stack_allocator<U>& rhs) noexcept;
}

#endif //VALLOCS_STACK_ALLOCATOR_H
