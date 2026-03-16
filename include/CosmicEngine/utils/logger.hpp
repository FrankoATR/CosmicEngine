#ifndef COSMIC_LOGGER_HPP
#define COSMIC_LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

namespace CosmicEngine {

enum class LogLevel { Info, Warning, Error };

class Logger {
public:
    static void init(bool enableConsole, bool enableFile);
    static void shutdown();

    static void log(LogLevel level, const std::string& msg);

    static void info(const std::string& msg);
    static void warning(const std::string& msg);
    static void error(const std::string& msg);

    static void setEnabled(bool enabled);
    static bool isEnabled();

private:
    static inline std::ofstream file;
    static inline bool consoleEnabled = true;
    static inline bool loggingEnabled = true;
    static inline std::mutex logMutex;

    static std::string generateLogFilename();
    static void ensureLogDirectory();
};

} // namespace CosmicEngine

#endif // COSMIC_LOGGER_HPP
