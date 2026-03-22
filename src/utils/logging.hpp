#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstdio>

namespace h323p {

// Log levels
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    NONE = 4
};

// Convert string to log level
inline LogLevel stringToLogLevel(const std::string& level) {
    if (level == "debug") return LogLevel::DEBUG;
    if (level == "info") return LogLevel::INFO;
    if (level == "warn") return LogLevel::WARN;
    if (level == "error") return LogLevel::ERROR;
    return LogLevel::INFO;
}

// Convert log level to string
inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// Format string helper (variadic)
template<typename... Args>
std::string formatString(const char* fmt, Args... args) {
    // Simple implementation using snprintf
    int size = std::snprintf(nullptr, 0, fmt, args...);
    if (size <= 0) return "";
    
    std::string result(size, '\0');
    std::snprintf(&result[0], size + 1, fmt, args...);
    return result;
}

// Overload for std::string format
inline std::string formatString(const std::string& fmt) {
    return fmt;
}

// Log entry structure
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
};

// Logger class (singleton with async support)
class Logger {
public:
    // Get singleton instance
    static Logger& instance();

    // Initialize logger
    void init(LogLevel level = LogLevel::INFO,
              const std::string& file = "",
              bool quiet = false);

    // Log methods
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);

    // Set log level
    void setLevel(LogLevel level);
    LogLevel getLevel() const { return level_; }

    // Shutdown logger (flush and stop async thread)
    void shutdown();

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Internal log method
    void log(LogLevel level, const std::string& msg);

    // Async logging thread
    void backgroundThread();

    LogLevel level_ = LogLevel::INFO;
    bool quiet_ = false;
    FILE* file_ = nullptr;
    
    // Async logging
    std::queue<LogEntry> queue_;
    std::mutex queueMutex_;
    std::condition_variable cv_;
    std::thread worker_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopRequested_{false};
};

// Convenience macros
#define LOG_DEBUG(msg) h323p::Logger::instance().debug(msg)
#define LOG_INFO(msg) h323p::Logger::instance().info(msg)
#define LOG_WARN(msg) h323p::Logger::instance().warn(msg)
#define LOG_ERROR(msg) h323p::Logger::instance().error(msg)

// Format macros (C++20 compatible)
#define LOG_DEBUG_FMT(fmt, ...) \
    h323p::Logger::instance().debug(h323p::formatString(fmt, ##__VA_ARGS__))
#define LOG_INFO_FMT(fmt, ...) \
    h323p::Logger::instance().info(h323p::formatString(fmt, ##__VA_ARGS__))
#define LOG_WARN_FMT(fmt, ...) \
    h323p::Logger::instance().warn(h323p::formatString(fmt, ##__VA_ARGS__))
#define LOG_ERROR_FMT(fmt, ...) \
    h323p::Logger::instance().error(h323p::formatString(fmt, ##__VA_ARGS__))

} // namespace h323p
