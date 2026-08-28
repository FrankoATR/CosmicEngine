#include "sysinfo.hpp"

#include <cstdlib>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define PSAPI_VERSION 2
#include <windows.h>
#include <psapi.h>
#endif

#if defined(_WIN32) && !defined(BENCH_IGPU)
// Ask hybrid-graphics drivers (NVIDIA Optimus / AMD PowerXpress) to run this
// process on the dedicated GPU. Exported from the executable, read by the driver.
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace Bench
{
    double ProcessAgeMs()
    {
#ifdef _WIN32
        FILETIME creation, exitT, kernel, user, now;
        if (!GetProcessTimes(GetCurrentProcess(), &creation, &exitT, &kernel, &user))
            return -1.0;
        GetSystemTimeAsFileTime(&now);
        ULARGE_INTEGER a, b;
        a.LowPart = creation.dwLowDateTime; a.HighPart = creation.dwHighDateTime;
        b.LowPart = now.dwLowDateTime;      b.HighPart = now.dwHighDateTime;
        return static_cast<double>(b.QuadPart - a.QuadPart) / 10000.0; // 100 ns units -> ms
#else
        return -1.0;
#endif
    }

    MemInfo QueryMemory()
    {
        MemInfo m;
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS_EX pmc{};
        pmc.cb = sizeof(pmc);
        if (K32GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc)))
        {
            m.workingSet = pmc.WorkingSetSize;
            m.privateBytes = pmc.PrivateUsage;
            m.peakWorkingSet = pmc.PeakWorkingSetSize;
        }
#endif
        return m;
    }

    std::string GetEnvOr(const char *name, const std::string &fallback)
    {
        const char *v = std::getenv(name);
        return (v && *v) ? std::string(v) : fallback;
    }
}
