#include <iostream>
#include <sys/types.h>
#include <sys/sysctl.h>

void printCPUInfo() {
    // Fetching core and thread count
    size_t size;
    sysctlbyname("hw.physicalcpu_max", nullptr, &size, nullptr, 0);
    int physicalCores;
    sysctlbyname("hw.physicalcpu_max", &physicalCores, &size, nullptr, 0);

    sysctlbyname("machdep.cpu.thread_count", nullptr, &size, nullptr, 0);
    int logicalCores;
    sysctlbyname("machdep.cpu.thread_count", &logicalCores, &size, nullptr, 0);

    std::cout << "Physical cores: " << physicalCores << std::endl;
    std::cout << "Logical cores: " << logicalCores << std::endl;

    // Checking for hyperthreading
    bool hyperthreadingPossible = (logicalCores > physicalCores);
    std::cout << "Hyperthreading supported: " << (hyperthreadingPossible ? "Yes" : "No") << std::endl;

    // Fetching cache line size
    int cacheLineSize;
    size = sizeof(cacheLineSize);
    sysctlbyname("hw.cachelinesize", &cacheLineSize, &size, nullptr, 0);
    std::cout << "Cache Line Size: " << cacheLineSize << " bytes" << std::endl;

    // Check for SIMD instruction support
    // ...

    // Check for other CPU information
    // ...
    bool hasNEON = false;
    size = sizeof(hasNEON);
    sysctlbyname("hw.optional.neon", &hasNEON, &size, nullptr, 0);

    if (hasNEON) {
        std::cout << "NEON SIMD instructions are supported." << std::endl;
    } else {
        std::cout << "NEON SIMD instructions are not supported." << std::endl;
    }




}


int main() {
    printCPUInfo();
    return 0;
}
