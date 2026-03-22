#pragma once

#include <future>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>

#include "utils/logging.hpp"

namespace h323p {

/**
 * Timeout exception
 */
class TimeoutException : public std::runtime_error {
public:
    explicit TimeoutException(const std::string& msg)
        : std::runtime_error(msg) {}
};

/**
 * Execute a function with a timeout
 * 
 * @param func Function to execute
 * @param timeout Timeout duration
 * @param operationName Name of the operation for logging
 * @return std::optional<T> with result if successful, std::nullopt if timeout
 * 
 * @tparam T Return type of the function
 * @tparam Func Function type
 * 
 * Example:
 * @code
 * auto result = executeWithTimeout(
 *     []() { return someLongOperation(); },
 *     std::chrono::seconds(5),
 *     "Some operation"
 * );
 * if (!result.has_value()) {
 *     // Timeout occurred
 * }
 * @endcode
 */
template<typename T, typename Func>
std::optional<T> executeWithTimeout(Func&& func, 
                                     std::chrono::milliseconds timeout,
                                     const std::string& operationName) {
    LOG_DEBUG_FMT("Starting operation '{}' with timeout {}ms", 
                  operationName, timeout.count());
    
    auto future = std::async(std::launch::async, std::forward<Func>(func));
    
    if (future.wait_for(timeout) == std::future_status::timeout) {
        LOG_WARN_FMT("Operation '{}' timed out after {}ms", 
                     operationName, timeout.count());
        return std::nullopt;
    }
    
    try {
        T result = future.get();
        LOG_DEBUG_FMT("Operation '{}' completed successfully", operationName);
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Operation '{}' failed: {}", operationName, e.what());
        throw;
    }
}

/**
 * Execute a void function with a timeout
 * 
 * @param func Function to execute
 * @param timeout Timeout duration
 * @param operationName Name of the operation for logging
 * @return true if successful, false if timeout
 */
inline bool executeWithTimeout(std::function<void()> func,
                                std::chrono::milliseconds timeout,
                                const std::string& operationName) {
    LOG_DEBUG_FMT("Starting operation '{}' with timeout {}ms",
                  operationName, timeout.count());
    
    auto future = std::async(std::launch::async, func);
    
    if (future.wait_for(timeout) == std::future_status::timeout) {
        LOG_WARN_FMT("Operation '{}' timed out after {}ms",
                     operationName, timeout.count());
        return false;
    }
    
    try {
        future.get();
        LOG_DEBUG_FMT("Operation '{}' completed successfully", operationName);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Operation '{}' failed: {}", operationName, e.what());
        throw;
    }
}

/**
 * Try to execute a function with a timeout, returns bool instead of throwing
 * 
 * @param func Function to execute
 * @param timeout Timeout duration
 * @param operationName Name of the operation for logging
 * @return true if successful, false if timeout or exception
 */
template<typename T, typename Func>
bool tryExecuteWithTimeout(Func&& func,
                           std::chrono::milliseconds timeout,
                           const std::string& operationName,
                           T& result) {
    LOG_DEBUG_FMT("Starting operation '{}' with timeout {}ms",
                  operationName, timeout.count());
    
    auto future = std::async(std::launch::async, std::forward<Func>(func));
    
    if (future.wait_for(timeout) == std::future_status::timeout) {
        LOG_WARN_FMT("Operation '{}' timed out after {}ms",
                     operationName, timeout.count());
        return false;
    }
    
    try {
        result = future.get();
        LOG_DEBUG_FMT("Operation '{}' completed successfully", operationName);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Operation '{}' failed: {}", operationName, e.what());
        return false;
    }
}

/**
 * Try to execute a void function with a timeout
 * 
 * @param func Function to execute
 * @param timeout Timeout duration
 * @param operationName Name of the operation for logging
 * @return true if successful, false if timeout or exception
 */
inline bool tryExecuteWithTimeout(std::function<void()> func,
                                   std::chrono::milliseconds timeout,
                                   const std::string& operationName) {
    LOG_DEBUG_FMT("Starting operation '{}' with timeout {}ms",
                  operationName, timeout.count());
    
    auto future = std::async(std::launch::async, func);
    
    if (future.wait_for(timeout) == std::future_status::timeout) {
        LOG_WARN_FMT("Operation '{}' timed out after {}ms",
                     operationName, timeout.count());
        return false;
    }
    
    try {
        future.get();
        LOG_DEBUG_FMT("Operation '{}' completed successfully", operationName);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR_FMT("Operation '{}' failed: {}", operationName, e.what());
        return false;
    }
}

} // namespace h323p
