#ifndef COSMIC_LOGGER_HPP
#define COSMIC_LOGGER_HPP

/**
 * @file logger.hpp
 * @brief Declares the thread-safe logger used by the engine runtime.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

namespace CosmicEngine {

/**
 * @brief Supported logging severity levels.
 */
enum class LogLevel { Info, Warning, Error };

/**
 * @brief Provides thread-safe console and file logging helpers.
 *
 * Logger is a static utility that writes timestamped log entries to the console
 * and/or a file inside the @c logs/ directory. The directory is created lazily
 * on first file-log initialization through @c std::filesystem::create_directories,
 * so the build system does not need to pre-create it. In @c Release builds
 * informational and warning logs are suppressed, while fatal errors still
 * surface as exceptions. Initialize it early in main() and shut it down before
 * exiting.
 *
 * @par Example â€” typical usage in main.cpp
 * @code
 * CosmicEngine::Logger::init(true, true);   // console + file
 * CosmicEngine::Logger::info("***** [PROGRAM START] *****");
 *
 * // ... engine loop ...
 *
 * CosmicEngine::Logger::info("**** [END PROGRAM SUCCESSFULLY] ****");
 * CosmicEngine::Logger::shutdown();
 * @endcode
 */
class Logger {
public:
    /**
     * @brief Initializes the logger outputs.
     * @param enableConsole True to emit logs to the console.
     * @param enableFile True to emit logs to a file.
     */
    static void init(bool enableConsole, bool enableFile);
    /** @brief Shuts the logger down and flushes open outputs. */
    static void shutdown();

    /**
     * @brief Writes a log entry with an explicit severity level.
     * @param level Entry severity.
     * @param msg Message text.
     */
    static void log(LogLevel level, const std::string& msg);

    /** @brief Writes an informational log entry. */
    static void info(const std::string& msg);

    /** @brief Writes a standardized lifecycle message for a runtime component. */
    static void lifecycle(const std::string& component, const std::string& action);
    /** @brief Writes a warning log entry. */
    static void warning(const std::string& msg);
    /** @brief Writes an error log entry. */
    static void error(const std::string& msg);
    /** @brief Returns whether the current build emits runtime logs. */
    static bool isRuntimeLoggingAvailable();

    /**
     * @brief Enables or disables logging globally.
     * @param enabled True to enable logging.
     */
    static void setEnabled(bool enabled);
    /** @brief Returns whether logging is currently enabled. */
    static bool isEnabled();

private:
    static inline std::ofstream file;
    static inline bool consoleEnabled = true;
    static inline bool loggingEnabled = true;
    static inline std::mutex logMutex;

    static std::string getCurrentTimestamp();
    static std::string generateLogFilename();
    static void ensureLogDirectory();
};

} // namespace CosmicEngine

#endif // COSMIC_LOGGER_HPP
