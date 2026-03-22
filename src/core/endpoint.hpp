#pragma once

#include <string>
#include <functional>
#include <map>
#include <mutex>

#include "h323/call_states.hpp"
#include "h323/h323_errors.hpp"
#include "h323/h323_endpoint.hpp"

namespace h323p {

// Forward declarations
class Call;

// Re-export types from h323 module for backward compatibility
using EndpointConfig = H323Config;
using EventType = H323EventType;
using Event = H323Event;
using EventCallback = H323EventCallback;

/**
 * H.323 Endpoint - wrapper around H323Endpoint (h323 module)
 * 
 * This is a higher-level wrapper that integrates with the Call class
 */
class Endpoint {
public:
    // Get singleton instance
    static Endpoint& instance();

    // Initialization
    bool initialize(const EndpointConfig& config);
    void shutdown();
    bool isInitialized() const { return initialized_; }

    // Gatekeeper
    bool registerWithGatekeeper(const std::string& gkAddress);
    bool unregisterFromGatekeeper();
    bool isRegisteredWithGatekeeper() const;

    // Calls
    bool makeCall(const std::string& destination, std::string& callId);
    bool answerCall(const std::string& callId);
    bool releaseCall(const std::string& callId, int cause = 16);

    // Callback
    void setEventCallback(EventCallback callback);

    // Statistics
    size_t getActiveCalls() const;
    size_t getTotalCalls() const { return totalCalls_; }

    // Get underlying H323Endpoint
    H323Endpoint& getH323Endpoint() { return H323Endpoint::instance(); }

private:
    Endpoint();
    ~Endpoint();

    Endpoint(const Endpoint&) = delete;
    Endpoint& operator=(const Endpoint&) = delete;

    EndpointConfig config_;
    EventCallback eventCallback_;
    bool initialized_ = false;
    size_t totalCalls_ = 0;

    // Active calls map
    std::map<std::string, Call*> activeCalls_;
    mutable std::mutex mutex_;
};

} // namespace h323p
