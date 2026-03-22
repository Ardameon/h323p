#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

#include "h323/call_states.hpp"

namespace h323p {

// Forward declarations
class Call;

/**
 * Call statistics
 */
struct CallStats {
    size_t totalCalls = 0;
    size_t successfulCalls = 0;
    size_t failedCalls = 0;
    size_t activeCalls = 0;
    size_t completedCalls = 0;
    std::chrono::milliseconds totalDuration{0};
    std::chrono::milliseconds averageDuration{0};
};

/**
 * Call Manager - manages multiple H.323 calls
 * 
 * Thread-safe singleton for managing call lifecycle
 */
class CallManager {
public:
    /**
     * Get singleton instance
     */
    static CallManager& instance();

    // Configuration

    /**
     * Set maximum concurrent calls
     */
    void setMaxCalls(size_t max);

    /**
     * Get maximum concurrent calls
     */
    size_t getMaxCalls() const { return maxCalls_; }

    /**
     * Set repeat count for call scenarios
     */
    void setRepeatCount(size_t count);

    /**
     * Set cycle mode (repeat calls indefinitely)
     */
    void setCycleMode(bool enable);

    /**
     * Set call duration range (seconds)
     */
    void setCallDurationRange(int minSeconds, int maxSeconds);

    // Call management

    /**
     * Start outgoing call
     * @param destination Destination number/address
     * @return Call ID if successful, empty string on failure
     */
    std::string startOutgoingCall(const std::string& destination);

    /**
     * Start listening for incoming calls
     */
    void startListening();

    /**
     * Stop listening
     */
    void stopListening();

    /**
     * Check if listening
     */
    bool isListening() const { return listening_; }

    /**
     * Stop all calls
     */
    void stopAllCalls();

    /**
     * Add call to manager
     */
    void addCall(std::shared_ptr<Call> call);

    /**
     * Remove call from manager
     */
    void removeCall(const std::string& callId);

    /**
     * Get call by ID
     */
    std::shared_ptr<Call> getCall(const std::string& callId);

    /**
     * Get all active calls
     */
    std::vector<std::shared_ptr<Call>> getActiveCalls() const;

    /**
     * Get all calls (including completed)
     */
    std::vector<std::shared_ptr<Call>> getAllCalls() const;

    // Statistics

    /**
     * Get call statistics
     */
    CallStats getStats() const;

    /**
     * Get number of active calls
     */
    size_t getActiveCallCount() const;

    /**
     * Get total number of calls
     */
    size_t getTotalCalls() const { return totalCalls_; }

    /**
     * Get number of completed calls
     */
    size_t getCompletedCalls() const { return completedCalls_.size(); }

    /**
     * Get number of failed calls
     */
    size_t getFailedCalls() const { return failedCalls_; }

    // Callbacks

    /**
     * Set call state callback
     */
    using CallStateCallback = std::function<void(const std::string& callId, CallState state)>;
    void setCallStateCallback(CallStateCallback callback);

private:
    CallManager();
    ~CallManager();

    CallManager(const CallManager&) = delete;
    CallManager& operator=(const CallManager&) = delete;

    void updateStats();

    std::vector<std::shared_ptr<Call>> activeCalls_;
    std::vector<std::shared_ptr<Call>> completedCalls_;
    
    mutable std::recursive_mutex mutex_;
    
    // Configuration
    size_t maxCalls_ = 10;
    size_t repeatCount_ = 1;
    bool cycleMode_ = false;
    int minCallDuration_ = 10;  // seconds
    int maxCallDuration_ = 30;  // seconds
    
    // State
    bool listening_ = false;
    size_t totalCalls_ = 0;
    size_t failedCalls_ = 0;
    
    // Statistics
    CallStats stats_;
    
    // Callbacks
    CallStateCallback stateCallback_;
};

} // namespace h323p
