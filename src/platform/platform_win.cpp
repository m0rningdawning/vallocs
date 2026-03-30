//
// Created by Paul on 08/11/2025.
//

#ifdef _WIN32
#include "platform.h"
#include <windows.h>

namespace platform {
    static std::size_t page_size() {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        return si.dwPageSize;
    }

    void* memory::reserve(const std::size_t bytes) {
        if (!bytes) return nullptr;
        void* p = VirtualAlloc(nullptr, bytes, MEM_RESERVE, PAGE_READWRITE);
        return p;
    }

    void* memory::commit(void* addr, const std::size_t bytes) {
        if (!addr || !bytes) return nullptr;
        return VirtualAlloc(addr, bytes, MEM_COMMIT, PAGE_READWRITE);
    }

    bool memory::decommit(void* addr, const std::size_t bytes) {
        if (!addr || !bytes) return false;
        return VirtualFree(addr, bytes, MEM_DECOMMIT) != 0;
    }

    bool memory::release(void* region) {
        if (!region) return false;
        return VirtualFree(region, 0, MEM_RELEASE) != 0;
    }
}

#endif
