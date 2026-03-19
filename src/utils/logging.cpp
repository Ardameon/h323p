#include "logging.hpp"
#include <iostream>
#include <cstring>

namespace h323p {

Logger::Logger() {
    // Start async logging thread
    running_ = true;
    worker_ = std::thread(&Logger::backgroundThread, this);
}

Logger::~Logger() {
    shutdown();
}

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::init(LogLevel level, const std::string& file, bool quiet) {
    level_ = level;
    quiet_ = quiet;

    if (!file.empty()) {
        if (file_) {
            fclose(file_);
            file_ = nullptr;
        }
        file_ = fopen(file.c_str(), "a");
        if (!file_) {
            // Fallback: print error to stderr
            fprintf(stderr, "Failed to open log file: %s (errno=%d)\n", file.c_str(), errno);
        }
    }
}

void Logger::shutdown() {
    // Signal stop
    stopRequested_ = true;
    cv_.notify_all();  // Wake up all waiting threads
    
    // Wait for worker thread
    if (worker_.joinable()) {
        worker_.join();
    }
    
    // Flush remaining logs
    if (file_) {
        fflush(file_);
        fclose(file_);
        file_ = nullptr;
    }
    
    running_ = false;
}

void Logger::backgroundThread() {
    while (running_ || !queue_.empty()) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        
        // Wait for new entries or stop signal
        cv_.wait_for(lock, std::chrono::milliseconds(100), [this]() {
            return !queue_.empty() || stopRequested_;
        });
        
        // Process queue
        while (!queue_.empty()) {
            LogEntry entry = queue_.front();
            queue_.pop();
            lock.unlock();
            
            // Format timestamp
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                entry.timestamp.time_since_epoch()) % 1000;

            std::ostringstream oss;
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            oss << " [" << logLevelToString(entry.level) << "] " << entry.message;

            std::string line = oss.str();

            // Output to console
            if (!quiet_) {
                if (entry.level >= LogLevel::WARN) {
                    std::cerr << line << std::endl;
                } else {
                    std::cout << line << std::endl;
                }
            }

            // Output to file
            if (file_) {
                fprintf(file_, "%s\n", line.c_str());
                fflush(file_);
            }
            
            lock.lock();
        }
        
        // Exit if stop requested and queue is empty
        if (stopRequested_ && queue_.empty()) {
            break;
        }
    }
}

void Logger::log(LogLevel level, const std::string& msg) {
    if (level < level_) {
        return;
    }

    // Create log entry
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.message = msg;

    // Add to queue
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        queue_.push(entry);
    }
    cv_.notify_one();
}

void Logger::debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
void Logger::info(const std::string& msg) { log(LogLevel::INFO, msg); }
void Logger::warn(const std::string& msg) { log(LogLevel::WARN, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::ERROR, msg); }

void Logger::setLevel(LogLevel level) { level_ = level; }

} // namespace h323p
