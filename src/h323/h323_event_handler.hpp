#pragma once

#include <functional>
#include <string>
#include <map>
#include <mutex>

#include "h323/call_states.hpp"
#include "h323/h323_errors.hpp"

// Forward declaration H3223Plus classes
class H323EndPoint;
class H323Connection;
class H323Listener;

namespace h323p {

/**
 * H.323 Event types
 */
enum class H323EventType {
    INCOMING_CALL,              // Incoming call received
    OUTGOING_CALL_CREATED,      // Outgoing call created
    CALL_ESTABLISHED,           // Call established (CONNECT)
    CALL_CLEARED,               // Call cleared (RELEASE COMPLETE)
    CALL_FAILED,                // Call setup failed
    GATEKEEPER_REGISTERED,      // Registered with GK
    GATEKEEPER_REGISTRATION_FAILED, // GK registration failed
    GATEKEEPER_DISCOVERED,      // GK discovered
    GATEKEEPER_DISCOVERY_FAILED, // GK discovery failed
    CALL_STATE_CHANGED          // Call state changed
};

/**
 * H.323 Event data
 */
struct H323Event {
    H323EventType type;
    std::string callId;
    std::string remoteAddress;
    std::string remoteNumber;
    std::string gatekeeperAddress;
    CallState callState = CallState::NULL_STATE;
    H323Error error = H323Error::SUCCESS;
    int errorCode = 0;
    std::string errorText;
};

/**
 * Event callback type
 */
using H323EventCallback = std::function<void(const H323Event&)>;

/**
 * H.323 Event Handler
 * 
 * Inherits from H323Listener to receive asynchronous events from H323Plus.
 * All callbacks are thread-safe and can be called from any thread.
 */
class H323EventHandler : public H323Listener {
public:
    H323EventHandler();
    ~H323EventHandler() override;

    /**
     * Set event callback
     */
    void setEventCallback(H323EventCallback callback);

    /**
     * Register endpoint for event handling
     */
    void setEndPoint(H323EndPoint* endpoint);

    // H323Listener interface implementation
    
    /**
     * Called when a new incoming connection is received
     */
    bool OnNewIncomingConnection(H323Connection* connection) override;

    /**
     * Called when connection state changes
     */
    void OnConnectionState(H323Connection* connection,
                          OpalConnection::CallResponseTypes response) override;

    /**
     * Called when connection is clearing
     */
    void OnConnectionClearing(H323Connection* connection,
                              const std::string& reason) override;

    /**
     * Called when call is established
     */
    void OnCallEstablished(H323Connection* connection) override;

    /**
     * Called when call is released
     */
    void OnCallReleased(H323Connection* connection) override;

    /**
     * Called when gatekeeper registration succeeds
     */
    void OnGatekeeperRegistered(bool success) override;

    /**
     * Called when gatekeeper is discovered
     */
    void OnGatekeeperDiscovered(bool success, const std::string& gkAddress) override;

private:
    /**
     * Send event to callback
     */
    void sendEvent(const H323Event& event);

    /**
     * Get call ID from connection
     */
    std::string getCallId(H323Connection* connection);

    /**
     * Get remote address from connection
     */
    std::string getRemoteAddress(H323Connection* connection);

    /**
     * Get remote number from connection
     */
    std::string getRemoteNumber(H323Connection* connection);

    H323EventCallback callback_;
    H323EndPoint* endpoint_;
    std::map<std::string, CallState> callStates_;
    mutable std::mutex mutex_;
};

} // namespace h323p
