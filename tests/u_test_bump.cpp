//
// Created by Paul on 27/10/2025.
//

#include <gtest/gtest.h>
#include <cstdint>
#include "allocs/bump/bump_allocator.h"

using ArenaT = vallocs::bump::bump_allocator<char>;

template <class A>
void* bump_alloc(A& a, std::size_t n, std::size_t align) {
#if __cpp_concepts
    if constexpr (requires(A x) { x.allocate(n, align); }) {
        return a.allocate(n, align);
    } else {
        (void)align;
        return a.allocate(n);
    }
#else
    return a.allocate(n);
#endif
}

template <class A>
void bump_reset(A& a) {
#if __cpp_concepts
    if constexpr (requires(A x) { x.reset(); }) {
        a.reset();
    } else if constexpr (requires(A x) { x.clear(); }) {
        a.clear();
    }
#endif
}

static bool is_aligned(void* p, std::size_t align) {
    return reinterpret_cast<std::uintptr_t>(p) % align == 0;
}

TEST(Arena, AllocateAndAlignment) {
    constexpr std::size_t capacity = 1024;
    constexpr std::size_t align = alignof(std::max_align_t);
    ArenaT bump(capacity);

    void* p = bump_alloc(bump, 128, align);
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(is_aligned(p, align));
}

TEST(Arena, BumpDistinctPointers) {
    constexpr std::size_t capacity = 1024;
    constexpr std::size_t align = alignof(std::max_align_t);
    ArenaT bump(capacity);

    void* a = bump_alloc(bump, 128, align);
    void* b = bump_alloc(bump, 128, align);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_NE(a, b);
}

TEST(Arena, ExhaustionReturnsNullptr) {
    constexpr std::size_t capacity = 256;
    constexpr std::size_t align = alignof(std::max_align_t);
    ArenaT bump(capacity);

    void* a = bump_alloc(bump, 128, align);
    ASSERT_NE(a, nullptr);
    void* b = bump_alloc(bump, 128, align);
    ASSERT_NE(b, nullptr);
    void* c = bump_alloc(bump, 16, align);
    EXPECT_EQ(c, nullptr); // should be out of space
}

TEST(Arena, ResetReusesSpace) {
    constexpr std::size_t capacity = 256;
    constexpr std::size_t align = alignof(std::max_align_t);
    ArenaT bump(capacity);

    void* first = bump_alloc(bump, 128, align);
    ASSERT_NE(first, nullptr);

    bump_reset(bump);

    void* after = bump_alloc(bump, 128, align);
    ASSERT_NE(after, nullptr);
    EXPECT_EQ(after, first);
}
