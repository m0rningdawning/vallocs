//
// Created by Paul on 08/11/2025.
//

#ifdef _WIN32
#include "platform.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// refactor this to a universal, alloc independent naming
namespace platform::bump {
    static std::size_t page_size() {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        return si.dwPageSize;
    }

    void* platform_memory::reserve(const std::size_t bytes) {
        if (!bytes) return nullptr;
        void* p = VirtualAlloc(nullptr, bytes, MEM_RESERVE, PAGE_READWRITE);
        return p;
    }

    void* platform_memory::commit(void* addr, const std::size_t bytes) {
        if (!addr || !bytes) return nullptr;
        return VirtualAlloc(addr, bytes, MEM_COMMIT, PAGE_READWRITE);
    }

    bool platform_memory::decommit(void* addr, const std::size_t bytes) {
        if (!addr || !bytes) return false;
        return VirtualFree(addr, bytes, MEM_DECOMMIT) != 0;
    }

    bool platform_memory::release(void* region) {
        if (!region) return false;
        return VirtualFree(region, 0, MEM_RELEASE) != 0;
    }
    bool platform_memory::release(void* addr, std::size_t /* bytes placeholder to keep the api unified */) {
        if (!addr) return false;
        return release(addr);
    }
}

#endif
