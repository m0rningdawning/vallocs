//
// Created by Paul on 08/11/2025.
//

#ifdef _WIN32
#include "platform.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace platform::arena {
    static std::size_t page_size() {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        return si.dwPageSize;
    }

    void* platform_memory::reserve(const std::size_t bytes) {
        void* p = VirtualAlloc(nullptr, bytes, MEM_RESERVE, PAGE_READWRITE);
        return p;
    }

    void* platform_memory::commit(void* addr, const std::size_t bytes) {
        return VirtualAlloc(addr, bytes, MEM_COMMIT, PAGE_READWRITE);
    }

    bool platform_memory::decommit(void* addr, const std::size_t bytes) {
        return VirtualFree(addr, bytes, MEM_DECOMMIT) != 0;
    }

    bool platform_memory::release(void* region) {
        if (!region) return true;
        return VirtualFree(region, 0, MEM_RELEASE) != 0;
    }
    bool platform_memory::release(void* addr, std::size_t /*bytes*/) {
        return platform_memory::release(addr);
    }
}

#endif
