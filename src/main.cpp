//
// Created by Paul on 27/10/2025.
//

#include <iostream>
#include <span>

#include "bump/bump_allocator.h"

int main () {
    srand(time(nullptr));
    vallocs::bump::bump_allocator<char> aa(1024);

    char* buf = aa.allocate(512);
    auto buf2 = aa.allocate_span(512);
    char* buf3 = aa.allocate(512);

    for (int i = 0; i < 512; ++i) {
        buf[i] = std::rand() % (122 - 97 + 1) + 97;
        buf2[i] = std::rand() % (122 - 97 + 1) + 97;
    }

    std::cout << "Buf 1:\n";
    for (int i = 0; i < 512; ++i) std::cout << buf[i];

    std::cout << "\nBuf 2:\n";
    for (const char c : buf2) std::cout << c;

    std::cout << "\n" << static_cast<void*>(buf) << "\n";
    std::cout << static_cast<void*>(buf2.data())<< "\n";
    std::cout << static_cast<void*>(buf3) << "\n";
}