//
// Created by Paul on 27/10/2025.
//

#include <gtest/gtest.h>
#include <cstdint>
#include "../src/arena/arena_allocator.h"

using ArenaT = vallocs::arena::arena_allocator<char>;

template <class A>
void* arena_alloc(A& a, std::size_t n, std::size_t align) {
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
void arena_reset(A& a) {
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
    ArenaT arena(capacity);

    void* p = arena_alloc(arena, 128, align);
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(is_aligned(p, align));
}

TEST(Arena, BumpDistinctPointers) {
    constexpr std::size_t capacity = 1024;
    constexpr std::size_t align = alignof(std::max_align_t);
    ArenaT arena(capacity);

    void* a = arena_alloc(arena, 128, align);
    void* b = arena_alloc(arena, 128, align);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_NE(a, b);
}

TEST(Arena, ExhaustionReturnsNullptr) {
    constexpr std::size_t capacity = 256;
    constexpr std::size_t align = alignof(std::max_align_t);
    ArenaT arena(capacity);

    void* a = arena_alloc(arena, 128, align);
    ASSERT_NE(a, nullptr);
    void* b = arena_alloc(arena, 128, align);
    ASSERT_NE(b, nullptr);
    void* c = arena_alloc(arena, 16, align);
    EXPECT_EQ(c, nullptr); // should be out of space
}

TEST(Arena, ResetReusesSpace) {
    constexpr std::size_t capacity = 256;
    constexpr std::size_t align = alignof(std::max_align_t);
    ArenaT arena(capacity);

    void* first = arena_alloc(arena, 128, align);
    ASSERT_NE(first, nullptr);

    arena_reset(arena);

    void* after = arena_alloc(arena, 128, align);
    ASSERT_NE(after, nullptr);
    EXPECT_EQ(after, first);
}
