#include <gtest/gtest.h>
#include "allocs/stack/stack_allocator.h"

using StackT = vallocs::stack::stack_allocator<char>;

TEST(StackAllocator, AllocateAndDistinct) {
    StackT sa(1024);
    char* buf1 = sa.allocate(450);
    char* buf2 = sa.allocate(450);

    ASSERT_NE(buf1, nullptr);
    ASSERT_NE(buf2, nullptr);
    EXPECT_NE(buf1, buf2);
}

TEST(StackAllocator, ExhaustionReturnsNullptr) {
    StackT sa(1024);
    char* buf1 = sa.allocate(600);
    ASSERT_NE(buf1, nullptr);

    EXPECT_THROW(
        {
            auto ptr = sa.allocate(600);
            (void)ptr; // Silence unused warning if it miraculously doesn't throw
        },
        std::bad_alloc
    ); // Not enough space
}
