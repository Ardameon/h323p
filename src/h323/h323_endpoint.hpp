#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <mutex>

#include "h323/h323_event_handler.hpp"
#include "h323/h323_errors.hpp"

// Forward declaration H323Plus classes
class H323EndPoint;
class H323Connection;

namespace h323p {

/**
 * H.323 Endpoint configuration
 */
struct H323Config {
    std::string localAddress = "0.0.0.0";
    int tcpPort = 1720;
    int udpPort = 1720;
    bool gatekeeperRequired = false;
    std::string gatekeeperAddress;
    std::string username;
    std::string password;
    bool fastStart = true;
    bool h245Tunneling = true;
    int maxBandwidth = 0;  // 0 = unlimited
    std::string aliases;   // H.323 aliases (E.164, H.323 ID, etc.)
};

/**
 * H.323 Endpoint - singleton wrapper around H323Plus
 * 
 * Provides a clean interface to H323Plus library for making and
 * receiving H.323 calls.
 * 
 * Thread-safe: All public methods are protected by mutex.
 */
class H323Endpoint {
public:
    /**
     * Get singleton instance
     */
    static H323Endpoint& instance();

    /**
     * Initialize endpoint with configuration
     * @param config Endpoint configuration
     * @return true if successful
     */
    bool initialize(const H323Config& config);

    /**
     * Shutdown endpoint and release all resources
     */
    void shutdown();

    /**
     * Check if endpoint is initialized
     */
    bool isInitialized() const { return initialized_; }

    /**
     * Get current configuration
     */
    const H323Config& getConfig() const { return config_; }

    // Gatekeeper operations

    /**
     * Register with Gatekeeper
     * @param gkAddress Gatekeeper address (host:port or host)
     * @return true if successful
     */
    bool registerWithGatekeeper(const std::string& gkAddress);

    /**
     * Unregister from Gatekeeper
     * @return true if successful
     */
    bool unregisterFromGatekeeper();

    /**
     * Check if registered with Gatekeeper
     */
    bool isRegisteredWithGatekeeper() const;

    // Call operations

    /**
     * Make an outgoing call
     * @param destination Destination number or address
     * @param callId Output parameter for call ID
     * @return true if call was created successfully
     */
    bool makeCall(const std::string& destination, std::string& callId);

    /**
     * Answer an incoming call
     * @param callId Call ID to answer
     * @return true if call was answered successfully
     */
    bool answerCall(const std::string& callId);

    /**
     * Release a call
     * @param callId Call ID to release
     * @param cause Release cause (Q.931)
     * @return true if call was released successfully
     */
    bool releaseCall(const std::string& callId, int cause = 16);

    /**
     * Release all active calls
     */
    void releaseAllCalls();

    // Callback

    /**
     * Set event callback
     * @param callback Event callback function
     */
    void setEventCallback(H323EventCallback callback);

    // Statistics

    /**
     * Get number of active calls
     */
    size_t getActiveCalls() const;

    /**
     * Get total number of calls made/received
     */
    size_t getTotalCalls() const { return totalCalls_; }

    /**
     * Get call state by ID
     */
    CallState getCallState(const std::string& callId) const;

    /**
     * Get all active call IDs
     */
    std::vector<std::string> getActiveCallIds() const;

private:
    H323Endpoint();
    ~H323Endpoint();

    H323Endpoint(const H323Endpoint&) = delete;
    H323Endpoint& operator=(const H323Endpoint&) = delete;

    // Internal methods

    /**
     * Add call to active calls map
     */
    void addCall(const std::string& callId, H323Connection* connection);

    /**
     * Remove call from active calls map
     */
    void removeCall(const std::string& callId);

    /**
     * Get connection by call ID
     */
    H323Connection* getConnection(const std::string& callId) const;

    H323Config config_;
    H323EndPoint* endpoint_;
    std::shared_ptr<H323EventHandler> eventHandler_;
    H323EventCallback eventCallback_;
    bool initialized_;
    size_t totalCalls_;

    // Active calls map
    std::map<std::string, H323Connection*> activeCalls_;
    mutable std::mutex mutex_;
};

} // namespace h323p
