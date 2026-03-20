//
// Created by Paul on 08/11/2025.
//

#ifndef VALLOCS_PLATFORM_H
#define VALLOCS_PLATFORM_H
#include <cstddef>

namespace platform {
    class memory {
    public:
        static void *reserve(std::size_t bytes);
        static void *commit(void *addr, std::size_t bytes);
        static bool decommit(void *addr, std::size_t bytes);
#ifdef __linux__
        static bool release(void *addr, std::size_t bytes);
#endif
#ifdef WIN32_LEAN_AND_MEAN
        static bool release(void *region);
#endif
    };
}

#endif //VALLOCS_PLATFORM_H
