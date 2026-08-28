#pragma once
#include <cstddef>
#include <string>

// Process-level measurements (Windows). Kept in its own translation unit so that
// <windows.h> never meets the engine headers.
namespace Bench
{
    struct MemInfo
    {
        std::size_t workingSet = 0;
        std::size_t privateBytes = 0;
        std::size_t peakWorkingSet = 0;
    };

    double ProcessAgeMs();           // milliseconds since the OS created this process
    MemInfo QueryMemory();
    std::string GetEnvOr(const char *name, const std::string &fallback);
}
