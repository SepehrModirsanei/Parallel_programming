#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#endif

namespace st = std;

void cpuid(int CPUInfo[4], int function) {
#ifdef _WIN32
    __cpuid(CPUInfo, function);
#else
    __asm__ __volatile__(
        "cpuid"
        : "=a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3])
        : "a" (function)
    );
#endif
}

void printCPUInfo() {
    int CPUInfo[4] = {0};
    cpuid(CPUInfo, 0);
    int nIds = CPUInfo[0];

    char CPUBrandString[0x40] = {0};
    for (int i = 0; i <= nIds; ++i) {
        cpuid(CPUInfo, i);
        memcpy(CPUBrandString + i * 16, CPUInfo, sizeof(CPUInfo));
    }
    st::cout << "CPU Type: " << CPUBrandString << st::endl;

    // Get logical and physical core counts
    cpuid(CPUInfo, 1);
    int logicalCores = (CPUInfo[1] >> 16) & 0xff;
    int physicalCores = (CPUInfo[1] >> 26) & 0x3f;
    st::cout << "Number of Logical Cores: " << logicalCores << st::endl;
    st::cout << "Number of Physical Cores: " << physicalCores << st::endl;

    // Get cache information (for common Intel CPUs)
    int cacheInfo[4] = {0};
    cpuid(cacheInfo, 0x80000006);
    int cacheSizeKB = (cacheInfo[2] >> 16) & 0xFFFF;
    st::cout << "Cache Size: " << cacheSizeKB << " KB" << st::endl;

    // Get supported SIMD instructions
    bool hasSSE = false;
    bool hasAVX = false;
    cpuid(CPUInfo, 1);
    if (CPUInfo[3] & (1 << 25)) {
        hasSSE = true;
    }
    if (CPUInfo[2] & (1 << 28)) {
        hasAVX = true;
    }

    st::cout << "Supported SIMD Instructions:" << st::endl;
    if (hasSSE) {
        st::cout << "SSE" << st::endl;
    }
    if (hasAVX) {
        st::cout << "AVX" << st::endl;
    }
}

int main() {
    printCPUInfo();
    return 0;
}
