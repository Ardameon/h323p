#pragma once

#include <string>
#include <chrono>

namespace h323p {

// Call states
enum class CallState {
    IDLE,           // Initial state
    SETUP,          // Call initiated
    PROCEEDING,     // Call proceeding received
    ALERTING,       // Remote party is ringing
    CONNECTED,      // Call established
    RELEASING,      // Release requested
    RELEASED        // Call completed
};

// H.323 Call class
// TODO: Implement in Stage 2
class Call {
public:
    explicit Call(const std::string& callId, bool incoming = false);
    ~Call();

    // Call control
    bool setup(const std::string& destination);
    bool answer();
    bool release(int cause = 0);

    // State
    CallState getState() const { return state_; }
    std::string getCallId() const { return callId_; }
    bool isIncoming() const { return incoming_; }

    // Timing
    std::chrono::system_clock::time_point getStartTime() const { return startTime_; }
    std::chrono::system_clock::time_point getConnectTime() const { return connectTime_; }
    std::chrono::system_clock::time_point getReleaseTime() const { return releaseTime_; }

private:
    std::string callId_;
    bool incoming_;
    CallState state_ = CallState::IDLE;
    
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point connectTime_;
    std::chrono::system_clock::time_point releaseTime_;
};

} // namespace h323p
