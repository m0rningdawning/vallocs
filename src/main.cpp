//
// Created by Paul on 27/10/2025.
//

#include <iostream>
#include "arena/arena_allocator.h"

int main () {
    srand(time(nullptr));
    arena::arena_allocator<char> aa(1024);

    char* buf = aa.allocate(512);
    char* buf2 = aa.allocate(512);
    char* buf3 = aa.allocate(512);

    for (int i = 0; i < 512; ++i) {
        buf[i] = std::rand() % (122 - 97 + 1) + 97;
        buf2[i] = std::rand() % (122 - 97 + 1) + 97;
    }

    std::cout << "Buf 1:\n";
    // Need to make it templated + implement the ranges (begin, end)
    // for (const char c : buf) std::cout << c;
    for (int i = 0; i < 512; ++i) std::cout << buf[i];

    std::cout << "\nBuf 2:\n";
    // for (const char c : buf2) std::cout << c;
    for (int i = 0; i < 512; ++i) std::cout << buf2[i];

    std::cout << "\n" << static_cast<void*>(buf) << "\n";
    std::cout << static_cast<void*>(buf2) << "\n";
    std::cout << static_cast<void*>(buf3) << "\n";
}