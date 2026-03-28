//
// Created by Paul on 27/10/2025.
//

#include <iostream>
#include <span>
#include <sys/stat.h>

#include "allocs/bump/bump_allocator.h"
#include "allocs/stack/stack_allocator.h"
#include "allocs/pool/pool_allocator.h"
#include "allocs/free_list/fl_allocator.h"

enum class alocs_e {
    BUMP_ALLOCATOR = 1,
    STACK_ALLOCATOR = 2,
    POOL_ALLOCATOR = 3,
    FL_ALLOCATOR = 4,
};

void test_bump() {
    std::cout << "Bump/Arena Allocator Test:\n";
    srand(time(nullptr));
    vallocs::bump::bump_allocator<char> ba(1024);

    char* buf = ba.allocate(512);
    auto buf2 = ba.allocate_span(512);
    char* buf3 = ba.allocate(512);

    for (int i = 0; i < 512; ++i) {
        buf[i] = std::rand() % (122 - 97 + 1) + 97;
        buf2[i] = std::rand() % (122 - 97 + 1) + 97;
    }

    std::cout << "Buf 1:\n";
    for (int i = 0; i < 512; ++i) std::cout << buf[i];

    std::cout << "\nBuf 2:\n";
    for (const char c : buf2) std::cout << c;

    std::cout << "\n" << static_cast<void*>(buf) << "\n";
    std::cout << static_cast<void*>(buf2.data()) << "\n";
    std::cout << static_cast<void*>(buf3) << "\n";
}

void test_stack() {
    std::cout << "Stack Allocator Test:\n";
    srand(time(nullptr));
    vallocs::stack::stack_allocator<char> sa(1024);
    char* buf = sa.allocate(450);
    char* buf2 = sa.allocate(450);

    for (int i = 0; i < 450; ++i) {
        buf[i] = std::rand() % (122 - 97 + 1) + 97;
        buf2[i] = std::rand() % (122 - 97 + 1) + 97;
    }

    std::cout << "Buf 1:\n";
    for (int i = 0; i < 450; ++i) std::cout << buf[i];

    std::cout << "\nBuf 2:\n";
    for (int i = 0; i < 450; ++i) std::cout << buf2[i];

    std::cout << "\n" << static_cast<void*>(buf) << "\n";
    std::cout << static_cast<void*>(buf2) << "\n";
}

void test_pool() {
    std::cout << "Pool Allocator Test:\n";
    srand(time(nullptr));
    vallocs::pool::pool_allocator<char> pa(1024);
    char* buf = pa.allocate();
    char* buf2 = pa.allocate();
    char* buf3 = pa.allocate();
    char* buf4 = pa.allocate();

    for (int i = 0; i < 256; ++i) {
        buf[i] = std::rand() % (122 - 97 + 1) + 97;
        buf2[i] = std::rand() % (122 - 97 + 1) + 97;
        buf3[i] = std::rand() % (122 - 97 + 1) + 97;
        buf4[i] = std::rand() % (122 - 97 + 1) + 97;
    }

    std::cout << "Buf 1:\n";
    for (int i = 0; i < 256; ++i) std::cout << buf[i];

    std::cout << "\nBuf 2:\n";
    for (int i = 0; i < 256; ++i) std::cout << buf2[i];

    std::cout << "\nBuf 3:\n";
    for (int i = 0; i < 256; ++i) std::cout << buf3[i];

    std::cout << "\nBuf 4:\n";
    for (int i = 0; i < 256; ++i) std::cout << buf4[i];

    char* buf5 = pa.allocate();
    pa.free(buf3);
    buf5 = pa.allocate();

    std::cout << "\n" << static_cast<void*>(buf) << "\n";
    std::cout << static_cast<void*>(buf2) << "\n";
    std::cout << static_cast<void*>(buf3) << "\n";
    std::cout << static_cast<void*>(buf4) << "\n";
}

void test_fl() {}

int main() {
    int alloc {};
    bool chosen { false };
    do {
        std::cout << "Enter allocation type: " << std::flush;
        std::cin >> alloc;
        switch (alloc) {
            case static_cast<int>(alocs_e::BUMP_ALLOCATOR): {
                test_bump();
                chosen = true;
                break;
            }
            case static_cast<int>(alocs_e::STACK_ALLOCATOR): {
                test_stack();
                chosen = true;
                break;
            }
            case static_cast<int>(alocs_e::POOL_ALLOCATOR): {
                test_pool();
                chosen = true;
                break;
            }
            case static_cast<int>(alocs_e::FL_ALLOCATOR): {
                test_fl();
                chosen = true;
                break;
            }
        default: {
                std::cout << "No allocator chosen, please choose one of the allocators from the enum"  << "\n" ;
                break;
            }
        }
    } while (!chosen);
    return 0;
}