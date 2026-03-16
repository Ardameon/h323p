#pragma once

#include <string>
#include <functional>
#include <map>

namespace h323p {

// Forward declarations
class Call;

// H.323 Endpoint configuration
// TODO: Implement in Stage 2
struct EndpointConfig {
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
};

// H.323 Event types
enum class EventType {
    INCOMING_CALL,
    CALL_ESTABLISHED,
    CALL_CLEARED,
    CALL_FAILED,
    GATEKEEPER_REGISTERED,
    GATEKEEPER_REGISTRATION_FAILED
};

// H.323 Event
struct Event {
    EventType type;
    std::string callId;
    std::string remoteAddress;
    std::string remoteNumber;
    int errorCode = 0;
    std::string errorText;
};

// Event callback type
using EventCallback = std::function<void(const Event&)>;

// H.323 Endpoint - wrapper around H323Plus
// TODO: Implement in Stage 2
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
    bool releaseCall(const std::string& callId, int cause = 0);

    // Callback
    void setEventCallback(EventCallback callback);

    // Statistics
    size_t getActiveCalls() const;
    size_t getTotalCalls() const { return totalCalls_; }

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
};

} // namespace h323p
