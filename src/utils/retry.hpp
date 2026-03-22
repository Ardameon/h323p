#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <optional>
#include <cmath>

#include "utils/logging.hpp"

namespace h323p {

/**
 * Retry configuration structure
 */
struct RetryConfig {
    size_t maxRetries = 3;                    // Maximum number of retry attempts
    std::chrono::milliseconds baseDelay{100}; // Base delay between retries
    double backoffMultiplier = 2.0;           // Exponential backoff multiplier
    std::chrono::milliseconds maxDelay{10000}; // Maximum delay between retries
    bool jitter = true;                       // Add random jitter to delay
};

/**
 * Retry with exponential backoff
 * 
 * @param func Function to execute (should return bool - true for success)
 * @param config Retry configuration
 * @param operationName Name of the operation for logging
 * @return true if operation succeeded, false if all retries failed
 * 
 * Example:
 * @code
 * RetryConfig config;
 * config.maxRetries = 5;
 * config.baseDelay = std::chrono::milliseconds(100);
 * 
 * bool success = retryWithBackoff(
 *     []() { return someOperation(); },
 *     config,
 *     "Some operation"
 * );
 * @endcode
 */
template<typename Func>
bool retryWithBackoff(Func&& func,
                      const RetryConfig& config,
                      const std::string& operationName) {
    size_t attempt = 0;
    std::chrono::milliseconds currentDelay = config.baseDelay;
    
    while (attempt <= config.maxRetries) {
        try {
            LOG_DEBUG_FMT("Attempting operation '{}' (attempt {}/{})",
                         operationName, attempt + 1, config.maxRetries + 1);
            
            if (func()) {
                LOG_DEBUG_FMT("Operation '{}' succeeded on attempt {}",
                             operationName, attempt + 1);
                return true;
            }
            
            LOG_DEBUG_FMT("Operation '{}' failed on attempt {}, will retry",
                         operationName, attempt + 1);
            
        } catch (const std::exception& e) {
            LOG_WARN_FMT("Operation '{}' threw exception on attempt {}: {}",
                        operationName, attempt + 1, e.what());
        }
        
        attempt++;
        
        if (attempt <= config.maxRetries) {
            // Calculate delay with exponential backoff
            auto delayMs = currentDelay.count();
            
            // Add jitter if enabled (±25% random variation)
            if (config.jitter) {
                double jitterFactor = 0.75 + (static_cast<double>(rand() % 1000) / 2000.0);
                delayMs = static_cast<long long>(delayMs * jitterFactor);
            }
            
            // Cap at maxDelay
            if (delayMs > config.maxDelay.count()) {
                delayMs = config.maxDelay.count();
            }
            
            LOG_INFO_FMT("Retrying operation '{}' in {}ms (attempt {}/{})",
                        operationName, delayMs, attempt + 1, config.maxRetries + 1);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            
            // Exponential backoff for next iteration
            currentDelay = std::chrono::milliseconds(
                static_cast<long long>(currentDelay.count() * config.backoffMultiplier)
            );
        }
    }
    
    LOG_ERROR_FMT("Operation '{}' failed after {} attempts",
                  operationName, config.maxRetries + 1);
    return false;
}

/**
 * Retry with exponential backoff - returns optional value
 * 
 * @param func Function to execute (should return std::optional<T>)
 * @param config Retry configuration
 * @param operationName Name of the operation for logging
 * @return std::optional<T> with result if successful, std::nullopt if all retries failed
 */
template<typename T, typename Func>
std::optional<T> retryWithBackoff(Func&& func,
                                   const RetryConfig& config,
                                   const std::string& operationName) {
    size_t attempt = 0;
    std::chrono::milliseconds currentDelay = config.baseDelay;
    
    while (attempt <= config.maxRetries) {
        try {
            LOG_DEBUG_FMT("Attempting operation '{}' (attempt {}/{})",
                         operationName, attempt + 1, config.maxRetries + 1);
            
            auto result = func();
            
            if (result.has_value()) {
                LOG_DEBUG_FMT("Operation '{}' succeeded on attempt {}",
                             operationName, attempt + 1);
                return result;
            }
            
            LOG_DEBUG_FMT("Operation '{}' returned no value on attempt {}, will retry",
                         operationName, attempt + 1);
            
        } catch (const std::exception& e) {
            LOG_WARN_FMT("Operation '{}' threw exception on attempt {}: {}",
                        operationName, attempt + 1, e.what());
        }
        
        attempt++;
        
        if (attempt <= config.maxRetries) {
            // Calculate delay with exponential backoff
            auto delayMs = currentDelay.count();
            
            // Add jitter if enabled (±25% random variation)
            if (config.jitter) {
                double jitterFactor = 0.75 + (static_cast<double>(rand() % 1000) / 2000.0);
                delayMs = static_cast<long long>(delayMs * jitterFactor);
            }
            
            // Cap at maxDelay
            if (delayMs > config.maxDelay.count()) {
                delayMs = config.maxDelay.count();
            }
            
            LOG_INFO_FMT("Retrying operation '{}' in {}ms (attempt {}/{})",
                        operationName, delayMs, attempt + 1, config.maxRetries + 1);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            
            // Exponential backoff for next iteration
            currentDelay = std::chrono::milliseconds(
                static_cast<long long>(currentDelay.count() * config.backoffMultiplier)
            );
        }
    }
    
    LOG_ERROR_FMT("Operation '{}' failed after {} attempts",
                  operationName, config.maxRetries + 1);
    return std::nullopt;
}

/**
 * Simple retry without backoff (fixed delay)
 * 
 * @param func Function to execute (should return bool)
 * @param maxRetries Maximum number of retries
 * @param delay Delay between retries
 * @param operationName Name of the operation for logging
 * @return true if operation succeeded, false if all retries failed
 */
template<typename Func>
bool simpleRetry(Func&& func,
                 size_t maxRetries,
                 std::chrono::milliseconds delay,
                 const std::string& operationName) {
    RetryConfig config;
    config.maxRetries = maxRetries;
    config.baseDelay = delay;
    config.backoffMultiplier = 1.0;  // No exponential backoff
    config.jitter = false;
    config.maxDelay = delay;
    
    return retryWithBackoff(std::forward<Func>(func), config, operationName);
}

} // namespace h323p
