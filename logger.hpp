#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

enum class LogLevel { INFO = 0, WARNING = 1, ERROR = 2 };

class SimpleLogger {
public:
    static SimpleLogger& instance() {
        static SimpleLogger logger;
        return logger;
    }

    void setLevel(LogLevel level) {
        minLevel_ = level;
    }

    void log(LogLevel level, const std::string& msg) {
        if (level < minLevel_) return;  // Skip if below threshold

        std::lock_guard<std::mutex> lock(mutex_);

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time);

        std::cout << "[" << std::put_time(&tm, "%F %T") << "] "
                  << "[" << levelToString(level) << "] "
                  << msg << "\n";
    }

private:
    SimpleLogger() : minLevel_(LogLevel::INFO) {}
    std::mutex mutex_;
    LogLevel minLevel_;

    const char* levelToString(LogLevel level) {
        switch(level) {
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
        }
        return "UNKNOWN";
    }
};

#define LOG_INFO(msg)    SimpleLogger::instance().log(LogLevel::INFO, msg)
#define LOG_WARNING(msg) SimpleLogger::instance().log(LogLevel::WARNING, msg)
#define LOG_ERROR(msg)   SimpleLogger::instance().log(LogLevel::ERROR, msg)

#endif

// int main() {
//     SimpleLogger::instance().setLevel(LogLevel::WARNING);  // Only log WARNING and ERROR

//     LOG_INFO("You won't see this info");  
//     LOG_WARNING("But you will see this warning");
//     LOG_ERROR("And this error, too");

//     return 0;
// }
