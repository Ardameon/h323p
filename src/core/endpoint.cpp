#include "core/endpoint.hpp"
#include "core/call.hpp"
#include "utils/logging.hpp"

namespace h323p {

Endpoint::Endpoint()
    : initialized_(false)
    , totalCalls_(0) {
    LOG_DEBUG("Endpoint created");
}

Endpoint::~Endpoint() {
    LOG_DEBUG("Endpoint destructor called");
    shutdown();
}

Endpoint& Endpoint::instance() {
    static Endpoint endpoint;
    return endpoint;
}

bool Endpoint::initialize(const EndpointConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        LOG_WARN("Endpoint already initialized");
        return true;
    }

    LOG_INFO_FMT("Initializing Endpoint: %s:%d",
                 config.localAddress.c_str(), config.tcpPort);

    config_ = config;

    // Initialize underlying H323Endpoint
    if (!H323Endpoint::instance().initialize(config)) {
        LOG_ERROR("Failed to initialize H323Endpoint");
        return false;
    }

    initialized_ = true;
    LOG_INFO("Endpoint initialized successfully");

    return true;
}

void Endpoint::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return;
    }

    LOG_INFO("Shutting down Endpoint");

    // Release all calls
    for (auto& [callId, call] : activeCalls_) {
        delete call;
    }
    activeCalls_.clear();

    // Shutdown underlying H323Endpoint
    H323Endpoint::instance().shutdown();

    initialized_ = false;
    LOG_INFO("Endpoint shutdown complete");
}

bool Endpoint::registerWithGatekeeper(const std::string& gkAddress) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOG_ERROR("Endpoint not initialized");
        return false;
    }

    LOG_INFO_FMT("Registering with Gatekeeper: %s", gkAddress.c_str());

    // Use underlying H323Endpoint
    return H323Endpoint::instance().registerWithGatekeeper(gkAddress);
}

bool Endpoint::unregisterFromGatekeeper() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    LOG_INFO("Unregistering from Gatekeeper");

    return H323Endpoint::instance().unregisterFromGatekeeper();
}

bool Endpoint::isRegisteredWithGatekeeper() const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    return H323Endpoint::instance().isRegisteredWithGatekeeper();
}

bool Endpoint::makeCall(const std::string& destination, std::string& callId) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOG_ERROR("Endpoint not initialized");
        return false;
    }

    LOG_INFO_FMT("Making call to: %s", destination.c_str());

    // Use underlying H323Endpoint
    if (!H323Endpoint::instance().makeCall(destination, callId)) {
        LOG_ERROR("Failed to make call");
        return false;
    }

    // Create Call object
    Call* call = new Call(callId, false);  // false = outgoing
    activeCalls_[callId] = call;
    totalCalls_++;

    LOG_INFO_FMT("Call created: %s", callId.c_str());

    return true;
}

bool Endpoint::answerCall(const std::string& callId) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        LOG_ERROR("Endpoint not initialized");
        return false;
    }

    auto it = activeCalls_.find(callId);
    if (it == activeCalls_.end()) {
        LOG_ERROR_FMT("Call %s not found", callId.c_str());
        return false;
    }

    LOG_INFO_FMT("Answering call: %s", callId.c_str());

    // Use underlying H323Endpoint
    return H323Endpoint::instance().answerCall(callId);
}

bool Endpoint::releaseCall(const std::string& callId, int cause) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    auto it = activeCalls_.find(callId);
    if (it == activeCalls_.end()) {
        LOG_DEBUG_FMT("Call %s not found (may already be released)", callId.c_str());
        return false;
    }

    LOG_INFO_FMT("Releasing call: %s (cause: %d)", callId.c_str(), cause);

    // Use underlying H323Endpoint
    bool result = H323Endpoint::instance().releaseCall(callId, cause);

    // Delete Call object
    delete it->second;
    activeCalls_.erase(it);

    return result;
}

void Endpoint::setEventCallback(EventCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventCallback_ = std::move(callback);

    // Set callback on underlying H323Endpoint
    H323Endpoint::instance().setEventCallback(eventCallback_);
}

size_t Endpoint::getActiveCalls() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeCalls_.size();
}

} // namespace h323p
