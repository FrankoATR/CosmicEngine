#include "logger.hpp"
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace CosmicEngine
{

    void Logger::init(bool enableConsole, bool enableFile)
    {
        consoleEnabled = enableConsole;

        if (enableFile)
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
        if (file.is_open())
            file.close();
    }

    void Logger::setEnabled(bool enabled)
    {
        loggingEnabled = enabled;
    }

    bool Logger::isEnabled()
    {
        return loggingEnabled;
    }

    void Logger::log(LogLevel level, const std::string &msg)
    {
        if (!loggingEnabled)
            return;

        std::lock_guard<std::mutex> lock(logMutex);

        std::time_t t = std::time(nullptr);
        char timeBuf[20];
        std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", std::localtime(&t));

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

        std::string finalMsg = std::string("[") + timeBuf + "] " + prefix + msg;

        if (consoleEnabled)
        {
            if (level == LogLevel::Warning)
                std::cerr << finalMsg << std::endl;
            else
                std::cout << finalMsg << std::endl;
        }

        if (file.is_open())
        {
            file << finalMsg << std::endl;
        }

        if (level == LogLevel::Error)
        {
            throw std::runtime_error(finalMsg);
        }
    }

    void Logger::info(const std::string &msg) { log(LogLevel::Info, msg); }
    void Logger::warning(const std::string &msg) { log(LogLevel::Warning, msg); }
    void Logger::error(const std::string &msg) { log(LogLevel::Error, msg); }

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
