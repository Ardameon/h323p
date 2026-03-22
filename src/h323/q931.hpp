#pragma once

#include <string>
#include <map>
#include <mutex>
#include <functional>
#include <optional>
#include <vector>

#include "h323/call_states.hpp"
#include "h323/h323_errors.hpp"

namespace h323p {

/**
 * Call information
 */
struct CallInfo {
    std::string callId;
    std::string destination;
    std::string remoteAddress;
    CallState state = CallState::NULL_STATE;
    ReleaseCause releaseCause = ReleaseCause::NORMAL_CALL_CLEARING;
    bool incoming = false;
};

/**
 * Q.931 Manager
 * 
 * Manages H.225 Q.931 call signaling
 * 
 * TODO: Implement in Stage 2
 */
class Q931Manager {
public:
    Q931Manager();
    ~Q931Manager();

    /**
     * Create outgoing call
     * @param destination Destination number or address
     * @return Call ID if successful, empty string on failure
     */
    std::string createOutgoingCall(const std::string& destination);

    /**
     * Answer incoming call
     * @param callId Call ID to answer
     * @return true if successful
     */
    bool answerCall(const std::string& callId);

    /**
     * Reject incoming call
     * @param callId Call ID to reject
     * @param cause Rejection cause
     * @return true if successful
     */
    bool rejectCall(const std::string& callId, ReleaseCause cause = ReleaseCause::CALL_REJECTED);

    /**
     * Release call
     * @param callId Call ID to release
     * @param cause Release cause
     * @return true if successful
     */
    bool releaseCall(const std::string& callId, ReleaseCause cause = ReleaseCause::NORMAL_CALL_CLEARING);

    /**
     * Get call state
     * @param callId Call ID
     * @return Call state
     */
    CallState getCallState(const std::string& callId) const;

    /**
     * Get call info
     * @param callId Call ID
     * @return Call info (empty optional if not found)
     */
    std::optional<CallInfo> getCallInfo(const std::string& callId) const;

    /**
     * Get all active call IDs
     */
    std::vector<std::string> getActiveCallIds() const;

    /**
     * Get number of active calls
     */
    size_t getActiveCallCount() const;

    /**
     * Set call state callback
     */
    using CallStateCallback = std::function<void(const std::string& callId, CallState state)>;
    void setCallStateCallback(CallStateCallback callback);

private:
    void updateCallState(const std::string& callId, CallState newState);

    std::map<std::string, CallInfo> calls_;
    CallStateCallback stateCallback_;
    mutable std::mutex mutex_;
};

} // namespace h323p
