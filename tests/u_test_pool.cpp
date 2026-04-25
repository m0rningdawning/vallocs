#include <gtest/gtest.h>
#include <vector>
#include "allocs/pool/pool_allocator.h"

using PoolT = vallocs::pool::pool_allocator<char>;

TEST(PoolAllocator, AllocateAndDistinct) {
    PoolT pa(1024, 256);
    char* buf1 = pa.allocate();
    char* buf2 = pa.allocate();
    ASSERT_NE(buf1, nullptr);
    ASSERT_NE(buf2, nullptr);
    EXPECT_NE(buf1, buf2);
}

TEST(PoolAllocator, FreeAndReallocate) {
    PoolT pa(1024, 256);
    char* buf1 = pa.allocate();
    char* buf2 = pa.allocate();
    char* buf3 = pa.allocate();
    char* buf4 = pa.allocate();
    ASSERT_NE(buf3, nullptr);
    pa.free(buf3);
    char* buf5 = pa.allocate();
    ASSERT_NE(buf5, nullptr);
    EXPECT_EQ(buf3, buf5);
}

TEST(PoolAllocator, ExhaustionReturnsNullptr) {
    PoolT pa(1024, 256);
    char* b1 = pa.allocate();
    char* b2 = pa.allocate();
    char* b3 = pa.allocate();
    char* b4 = pa.allocate();
    char* b5 = pa.allocate();
    EXPECT_NE(b4, nullptr);
    EXPECT_EQ(b5, nullptr);
}
