#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string_view>
#include <thread>

#define USE_HUGEPAGES_MALLOC __linux__

#if USE_HUGEPAGES_MALLOC
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h> // streerror
#endif

//grep Huge /proc/meminfo # to validate

std::string hugepage_usage() {
#if USE_HUGEPAGES_MALLOC
    /* Check whether transparent huge pages have been allocated. */
    std::ifstream smapsFile("/proc/self/smaps");
    std::array<char, 4096> lineBuffer;
    while (smapsFile.good()) {
        /* Getline always appends null. */
        smapsFile.getline(lineBuffer.data(), lineBuffer.size(), '\n');
        std::string_view line{lineBuffer.data()};
        if (line.rfind("AnonHugePages:", 0) == 0 && line.find("0 kB") == std::string::npos){ return std::string(line); }
    }
#endif
    return "AnonHugePages:      0 kB";
}

void* hugepage_malloc(size_t memorySize) {
#if USE_HUGEPAGES_MALLOC
    void* data{nullptr};
    const auto memalignError =
        posix_memalign(&data, 2ULL * 1024ULL * 1024ULL, memorySize); /* alignment equal or higher to huge page size */
    if (memalignError != 0) {
        throw std::runtime_error(std::string("Failed to allocate memory: ") + strerror(memalignError));
    }

    if (madvise(data, memorySize, MADV_HUGEPAGE) != 0) {
        throw std::runtime_error(std::string("Error on madvise: ") + strerror(errno));
    }

    return data;
#else
    return malloc(memorySize);
#endif
}