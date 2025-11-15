//
// Created by Paul on 08/11/2025.
//

#ifndef VALLOCS_PLATFORM_H
#define VALLOCS_PLATFORM_H
#include <cstddef>

namespace platform::bump {
    class platform_memory {
    public:
        static void* reserve(std::size_t bytes);
        static void* commit(void* addr, std::size_t bytes);
        static bool decommit(void* addr, std::size_t bytes);
        static bool release(void* region);
        static bool release(void* addr, std::size_t bytes);
    };
}

namespace platform::stack {

}

#endif //VALLOCS_PLATFORM_H
