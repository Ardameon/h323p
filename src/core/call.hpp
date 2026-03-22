#pragma once

#include <string>
#include <chrono>
#include <mutex>

#include "h323/call_states.hpp"

namespace h323p {

// Re-export CallState from h323 module for backward compatibility
using CallState = h323p::CallState;

/**
 * H.323 Call class
 * 
 * Represents a single H.323 call with state tracking and statistics
 */
class Call {
public:
    explicit Call(const std::string& callId, bool incoming = false);
    ~Call();

    // Call control
    bool setup(const std::string& destination);
    bool answer();
    bool release(int cause = 16);

    // State
    CallState getState() const;
    std::string getCallId() const { return callId_; }
    bool isIncoming() const { return incoming_; }
    std::string getDestination() const { return destination_; }
    std::string getRemoteAddress() const { return remoteAddress_; }

    // State management (called by Endpoint/CallManager)
    void setState(CallState state);
    void setRemoteAddress(const std::string& address);

    // Timing
    std::chrono::system_clock::time_point getStartTime() const { return startTime_; }
    std::chrono::system_clock::time_point getConnectTime() const { return connectTime_; }
    std::chrono::system_clock::time_point getReleaseTime() const { return releaseTime_; }

    // Duration
    std::chrono::milliseconds getDuration() const;
    std::chrono::milliseconds getConnectDuration() const;

    // Release cause
    int getReleaseCause() const { return releaseCause_; }
    void setReleaseCause(int cause) { releaseCause_ = cause; }

    // Statistics
    size_t getBytesSent() const { return bytesSent_; }
    size_t getBytesReceived() const { return bytesReceived_; }
    void addBytesSent(size_t bytes) { bytesSent_ += bytes; }
    void addBytesReceived(size_t bytes) { bytesReceived_ += bytes; }

private:
    std::string callId_;
    bool incoming_;
    std::string destination_;
    std::string remoteAddress_;
    
    mutable std::mutex mutex_;
    CallState state_ = CallState::NULL_STATE;

    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point connectTime_;
    std::chrono::system_clock::time_point releaseTime_;
    
    int releaseCause_ = 0;
    
    // Statistics
    size_t bytesSent_ = 0;
    size_t bytesReceived_ = 0;
};

} // namespace h323p
