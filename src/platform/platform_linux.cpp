//
// Created by Paul on 08/11/2025.
//

#ifdef __linux__
#include "platform.h"
#include <sys/mman.h>
#include <unistd.h>
#include <cstddef>
#include <cstdint>

namespace platform::arena {
    static std::size_t page_size() {
        static std::size_t ps = static_cast<std::size_t>(::sysconf(_SC_PAGESIZE));
        return ps;
    }

    void* platform_memory::reserve(std::size_t bytes) {
        if (!bytes) return nullptr;
        void* p = ::mmap(nullptr, bytes, PROT_NONE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return (p == MAP_FAILED) ? nullptr : p;
    }

    void* platform_memory::commit(void* addr, std::size_t bytes) {
        if (!addr || !bytes) return nullptr;
        if (::mprotect(addr, bytes, PROT_READ | PROT_WRITE) != 0)
            return nullptr;
        return addr;
    }

    bool platform_memory::decommit(void* addr, std::size_t bytes) {
        if (!addr || !bytes) return false;
        ::madvise(addr, bytes, MADV_DONTNEED);
        return ::mprotect(addr, bytes, PROT_NONE) == 0;
    }

    bool platform_memory::release(void* addr, std::size_t bytes) {
        if (!addr || !bytes) return false;
        return ::munmap(addr, bytes) == 0;
    }
}

#endif