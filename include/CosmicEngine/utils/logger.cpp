#include "logger.hpp"
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace CosmicEngine
{

    bool Logger::isRuntimeLoggingAvailable()
    {
    #ifdef NDEBUG
        return false;
    #else
        return true;
    #endif
    }

    void Logger::init(bool enableConsole, bool enableFile)
    {
        std::lock_guard<std::mutex> lock(logMutex);

        consoleEnabled = enableConsole && isRuntimeLoggingAvailable();
        loggingEnabled = isRuntimeLoggingAvailable();

        if (file.is_open())
        {
            file.close();
        }

        if (enableFile && loggingEnabled)
        {
            ensureLogDirectory();
            std::string filename = generateLogFilename();
            file.open(filename, std::ios::out | std::ios::trunc);
            if (!file.is_open())
            {
                throw std::runtime_error("The log file could not be opened: " + filename);
            }
        }
    }

    void Logger::shutdown()
    {
        std::lock_guard<std::mutex> lock(logMutex);

        if (file.is_open())
        {
            file.flush();
            file.close();
        }
    }

    void Logger::setEnabled(bool enabled)
    {
        loggingEnabled = enabled && isRuntimeLoggingAvailable();
    }

    bool Logger::isEnabled()
    {
        return loggingEnabled && isRuntimeLoggingAvailable();
    }

    void Logger::log(LogLevel level, const std::string &msg)
    {
        const bool shouldEmit = isEnabled();

        if (!shouldEmit && level != LogLevel::Error)
            return;

        std::lock_guard<std::mutex> lock(logMutex);

        std::string prefix;
        switch (level)
        {
        case LogLevel::Info:
            prefix = "[Info] ";
            break;
        case LogLevel::Warning:
            prefix = "[Warning] ";
            break;
        case LogLevel::Error:
            prefix = "[Error] ";
            break;
        }

        std::string finalMsg = std::string("[") + getCurrentTimestamp() + "] " + prefix + msg;

        if (shouldEmit && consoleEnabled)
        {
            if (level == LogLevel::Warning)
                std::cerr << finalMsg << std::endl;
            else
                std::cout << finalMsg << std::endl;
        }

        if (shouldEmit && file.is_open())
        {
            file << finalMsg << std::endl;
            file.flush();
        }

        if (level == LogLevel::Error)
        {
            throw std::runtime_error(finalMsg);
        }
    }

    void Logger::info(const std::string &msg) { log(LogLevel::Info, msg); }
    void Logger::warning(const std::string &msg) { log(LogLevel::Warning, msg); }
    void Logger::error(const std::string &msg) { log(LogLevel::Error, msg); }
    void Logger::lifecycle(const std::string &component, const std::string &action) { info("[" + component + "] " + action); }

    std::string Logger::getCurrentTimestamp()
    {
        std::time_t t = std::time(nullptr);
        std::tm tm{};

    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        return oss.str();
    }

    std::string Logger::generateLogFilename()
    {
        std::time_t t = std::time(nullptr);
        std::tm tm{};

        #ifdef _WIN32
                localtime_s(&tm, &t);
        #else
                localtime_r(&t, &tm);
        #endif

        std::ostringstream oss;
        oss << "logs/"
            << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S")
            << ".log";

        return oss.str();
    }

    void Logger::ensureLogDirectory()
    {
        std::filesystem::create_directories("logs");
    }

} // namespace CosmicEngine
