#include "h323/h323_endpoint.hpp"
#include "utils/logging.hpp"

// H323Plus headers (will be available when H323Plus is built)
// #include <h323.h>
// #include <h323ep.h>
// #include <h323con.h>
// #include <h323gk.h>
// #include <ptlib/socket.h>

namespace h323p {

H323Endpoint::H323Endpoint()
    : endpoint_(nullptr)
    , eventHandler_(nullptr)
    , eventCallback_(nullptr)
    , initialized_(false)
    , totalCalls_(0) {
    LOG_DEBUG("H323Endpoint created");
}

H323Endpoint::~H323Endpoint() {
    LOG_DEBUG("H323Endpoint destructor called");
    shutdown();
}

H323Endpoint& H323Endpoint::instance() {
    static H323Endpoint endpoint;
    return endpoint;
}

bool H323Endpoint::initialize(const H323Config& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        LOG_WARN("H323Endpoint already initialized");
        return true;
    }

    LOG_INFO_FMT("Initializing H323Endpoint: %s:%d",
                 config.localAddress.c_str(), config.tcpPort);

    config_ = config;

    // Create H323EndPoint (H323Plus)
    // This is a placeholder - actual implementation will use H323Plus
    // endpoint_ = new H323EndPoint();

    // TODO: Configure endpoint
    // - Set listening address
    // - Configure Fast Start
    // - Configure H.245 Tunneling
    // - Set bandwidth limits
    // - Register event handler

    // Create and register event handler
    eventHandler_ = std::make_shared<H323EventHandler>();
    // endpoint_->AddListener(*eventHandler_);

    // Configure listening address
    // PIPSocket::Address bindAddress;
    // if (config.localAddress == "0.0.0.0") {
    //     bindAddress = PIPSocket::GetDefaultIpAny();
    // } else {
    //     bindAddress = PIPSocket::Address(config.localAddress.c_str());
    // }
    // endpoint_->SetPort(config.tcpPort);

    // Fast Start
    // endpoint_->SetFastStart(config.fastStart);

    // H.245 Tunneling
    // endpoint_->SetTunnelH245(config.h245Tunneling);

    // Bandwidth
    // if (config.maxBandwidth > 0) {
    //     endpoint_->SetInitialBandwidth(config.maxBandwidth);
    // }

    initialized_ = true;
    LOG_INFO("H323Endpoint initialized successfully");

    return true;
}

void H323Endpoint::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return;
    }

    LOG_INFO("Shutting down H323Endpoint");

    // Release all calls
    releaseAllCalls();

    // Unregister from gatekeeper if registered
    if (config_.gatekeeperRequired) {
        // endpoint_->UnregisterGatekeeper();
    }

    // Remove event handler
    // if (endpoint_ && eventHandler_) {
    //     endpoint_->RemoveListener(*eventHandler_);
    // }

    // Delete endpoint
    // delete endpoint_;
    endpoint_ = nullptr;

    eventHandler_.reset();
    activeCalls_.clear();
    initialized_ = false;

    LOG_INFO("H323Endpoint shutdown complete");
}

bool H323Endpoint::registerWithGatekeeper(const std::string& gkAddress) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOG_ERROR("H323Endpoint not initialized");
        return false;
    }

    LOG_INFO_FMT("Registering with Gatekeeper: %s", gkAddress.c_str());

    // TODO: Implement gatekeeper registration
    // H323RegistrationInfo info;
    // info.gatekeeperAddress = gkAddress.c_str();
    // info.username = config_.username.c_str();
    // info.password = config_.password.c_str();
    //
    // bool result = endpoint_->RegisterGatekeeper(info);

    // Placeholder
    bool result = true;

    if (result) {
        LOG_INFO("Gatekeeper registration successful");
    } else {
        LOG_ERROR("Gatekeeper registration failed");
    }

    return result;
}

bool H323Endpoint::unregisterFromGatekeeper() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    LOG_INFO("Unregistering from Gatekeeper");

    // TODO: Implement gatekeeper unregistration
    // return endpoint_->UnregisterGatekeeper();

    return true;
}

bool H323Endpoint::isRegisteredWithGatekeeper() const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    // TODO: Implement gatekeeper status check
    // return endpoint_->IsRegisteredWithGatekeeper();

    return !config_.gatekeeperRequired;
}

bool H323Endpoint::makeCall(const std::string& destination, std::string& callId) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOG_ERROR("H323Endpoint not initialized");
        return false;
    }

    LOG_INFO_FMT("Making call to: %s", destination.c_str());

    // TODO: Implement outgoing call
    // PString token = destination.c_str();
    // PString alias = config_.username.c_str();
    //
    // H323Connection* connection = endpoint_->MakeCall(token, alias);
    //
    // if (connection) {
    //     callId = getCallId(connection);
    //     addCall(callId, connection);
    //     totalCalls_++;
    //     return true;
    // }

    // Placeholder
    callId = "call-" + std::to_string(totalCalls_);
    totalCalls_++;

    LOG_INFO_FMT("Call created (placeholder): %s", callId.c_str());
    return true;
}

bool H323Endpoint::answerCall(const std::string& callId) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOG_ERROR("H323Endpoint not initialized");
        return false;
    }

    H323Connection* connection = getConnection(callId);
    if (!connection) {
        LOG_ERROR_FMT("Call %s not found", callId.c_str());
        return false;
    }

    LOG_INFO_FMT("Answering call: %s", callId.c_str());

    // TODO: Implement call answer
    // return connection->AnswerCall();

    return true;
}

bool H323Endpoint::releaseCall(const std::string& callId, int cause) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    H323Connection* connection = getConnection(callId);
    if (!connection) {
        LOG_DEBUG_FMT("Call %s not found (may already be released)", callId.c_str());
        return false;
    }

    LOG_INFO_FMT("Releasing call: %s (cause: %d)", callId.c_str(), cause);

    // TODO: Implement call release
    // bool result = connection->Release(cause);
    // removeCall(callId);
    // return result;

    removeCall(callId);
    return true;
}

void H323Endpoint::releaseAllCalls() {
    // Note: Don't hold lock here - removeCall acquires lock
    std::vector<std::string> callIds;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [id, conn] : activeCalls_) {
            callIds.push_back(id);
        }
    }

    for (const auto& callId : callIds) {
        releaseCall(callId, 16);  // Normal call clearing
    }
}

void H323Endpoint::setEventCallback(H323EventCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventCallback_ = std::move(callback);

    if (eventHandler_) {
        eventHandler_->setEventCallback(eventCallback_);
    }

    LOG_DEBUG("Event callback set");
}

size_t H323Endpoint::getActiveCalls() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeCalls_.size();
}

CallState H323Endpoint::getCallState(const std::string& callId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = activeCalls_.find(callId);
    if (it == activeCalls_.end()) {
        return CallState::NULL_STATE;
    }

    // TODO: Get actual state from connection
    // return getConnectionState(it->second);

    return CallState::CALL_ACTIVE;
}

std::vector<std::string> H323Endpoint::getActiveCallIds() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> ids;
    for (const auto& [id, conn] : activeCalls_) {
        ids.push_back(id);
    }
    return ids;
}

void H323Endpoint::addCall(const std::string& callId, H323Connection* connection) {
    // Note: Caller must hold lock
    activeCalls_[callId] = connection;
    LOG_DEBUG_FMT("Call added: %s (total: %zu)", callId.c_str(), activeCalls_.size());
}

void H323Endpoint::removeCall(const std::string& callId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = activeCalls_.find(callId);
    if (it != activeCalls_.end()) {
        activeCalls_.erase(it);
        LOG_DEBUG_FMT("Call removed: %s (total: %zu)", callId.c_str(), activeCalls_.size());
    }
}

H323Connection* H323Endpoint::getConnection(const std::string& callId) const {
    // Note: Caller must hold lock
    auto it = activeCalls_.find(callId);
    return (it != activeCalls_.end()) ? it->second : nullptr;
}

} // namespace h323p
