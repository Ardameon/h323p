#pragma once

#include <functional>
#include <string>
#include <map>
#include <mutex>

#include "h323/call_states.hpp"
#include "h323/h323_errors.hpp"

// Forward declaration H323Plus classes
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
 * Handles asynchronous events from H323Plus.
 * Note: Actual H323Listener integration requires H323Plus.
 */
class H323EventHandler {
public:
    H323EventHandler();
    ~H323EventHandler();

    /**
     * Set event callback
     */
    void setEventCallback(H323EventCallback callback);

    /**
     * Register endpoint for event handling
     */
    void setEndPoint(H323EndPoint* endpoint);

    // Event handlers (called by H323Endpoint)
    
    /**
     * Called when a new incoming connection is received
     */
    bool onNewIncomingConnection(H323Connection* connection);

    /**
     * Called when connection state changes
     */
    void onConnectionState(H323Connection* connection, int response);

    /**
     * Called when connection is clearing
     */
    void onConnectionClearing(H323Connection* connection, const std::string& reason);

    /**
     * Called when call is established
     */
    void onCallEstablished(H323Connection* connection);

    /**
     * Called when call is released
     */
    void onCallReleased(H323Connection* connection);

    /**
     * Called when gatekeeper registration succeeds
     */
    void onGatekeeperRegistered(bool success);

    /**
     * Called when gatekeeper is discovered
     */
    void onGatekeeperDiscovered(bool success, const std::string& gkAddress);

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
