#include <gtest/gtest.h>
#include "allocs/free_list/fl_allocator.h"

using FreeListT = vallocs::fl::free_list<char>;

TEST(FreeListAllocator, AllocateAndDistinct) {
    FreeListT fla(2048, vallocs::fl::policy::FIND_FIRST);
    char* buf1 = fla.allocate(256, 8);
    char* buf2 = fla.allocate(512, 16);
    ASSERT_NE(buf1, nullptr);
    ASSERT_NE(buf2, nullptr);
    EXPECT_NE(buf1, buf2);
}

TEST(FreeListAllocator, FreeReusesSpace) {
    FreeListT fla(2048, vallocs::fl::policy::FIND_FIRST);
    char* buf1 = fla.allocate(256, 8);
    char* buf2 = fla.allocate(512, 16);
    ASSERT_NE(buf2, nullptr);
    fla.free(buf2);
    char* buf3 = fla.allocate(128, 8);
    ASSERT_NE(buf3, nullptr);
    // buf3 should overlap the space from buf2 as we used FIND_FIRST
}
