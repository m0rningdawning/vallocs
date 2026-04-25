#include <gtest/gtest.h>
#include "allocs/buddy/buddy_allocator.h"

using BuddyT = vallocs::buddy::buddy_allocator<char>;

TEST(BuddyAllocator, AllocateAndDistinct) {
    BuddyT ba(1024, 16);
    char* buf1 = ba.allocate(128, 16);
    char* buf2 = ba.allocate(256, 16);
    ASSERT_NE(buf1, nullptr);
    ASSERT_NE(buf2, nullptr);
    EXPECT_NE(buf1, buf2);
}

TEST(BuddyAllocator, BuddyFreeAndReallocate) {
    BuddyT ba(1024, 16);
    char* buf1 = ba.allocate(128, 16);
    char* buf2 = ba.allocate(256, 16);
    char* buf3 = ba.allocate(128, 16);
    ASSERT_NE(buf2, nullptr);
    ba.free(buf2);
    char* buf4 = ba.allocate(256, 16);
    ASSERT_NE(buf4, nullptr);
    EXPECT_EQ(buf2, buf4);
}
