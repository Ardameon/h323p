#include "h323/h323_event_handler.hpp"
#include "utils/logging.hpp"

// H323Plus headers (will be available when H323Plus is built)
// #include <h323.h>
// #include <h323ep.h>
// #include <h323con.h>
// #include <h323gk.h>

namespace h323p {

H323EventHandler::H323EventHandler()
    : endpoint_(nullptr) {
    LOG_DEBUG("H323EventHandler created");
}

H323EventHandler::~H323EventHandler() {
    LOG_DEBUG("H323EventHandler destroyed");
}

void H323EventHandler::setEventCallback(H323EventCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    callback_ = std::move(callback);
    LOG_DEBUG("Event callback registered");
}

void H323EventHandler::setEndPoint(H323EndPoint* endpoint) {
    std::lock_guard<std::mutex> lock(mutex_);
    endpoint_ = endpoint;
    LOG_DEBUG("Endpoint registered with event handler");
}

bool H323EventHandler::OnNewIncomingConnection(H323Connection* connection) {
    if (!connection) {
        LOG_ERROR("OnNewIncomingConnection called with null connection");
        return false;
    }

    std::string callId = getCallId(connection);
    std::string remoteAddr = getRemoteAddress(connection);
    std::string remoteNum = getRemoteNumber(connection);

    LOG_INFO_FMT("Incoming call: %s from %s (%s)",
                 callId.c_str(), remoteAddr.c_str(), remoteNum.c_str());

    {
        std::lock_guard<std::mutex> lock(mutex_);
        callStates_[callId] = CallState::CALL_RECEIVED;
    }

    H323Event event;
    event.type = H323EventType::INCOMING_CALL;
    event.callId = callId;
    event.remoteAddress = remoteAddr;
    event.remoteNumber = remoteNum;
    event.callState = CallState::CALL_RECEIVED;

    sendEvent(event);

    // Return true to accept the call
    return true;
}

void H323EventHandler::OnConnectionState(H323Connection* connection,
                                         OpalConnection::CallResponseTypes response) {
    if (!connection) {
        return;
    }

    std::string callId = getCallId(connection);
    CallState newState = CallState::NULL_STATE;

    // Map Opal response to CallState
    switch (response) {
        case OpalConnection::CallResponseInProgress:
            newState = CallState::CALL_PROCEEDING;
            LOG_DEBUG_FMT("Call %s: PROCEEDING", callId.c_str());
            break;

        case OpalConnection::CallResponseAlerting:
            newState = CallState::CALL_ALERTING;
            LOG_DEBUG_FMT("Call %s: ALERTING", callId.c_str());
            break;

        case OpalConnection::CallResponseConnect:
            newState = CallState::CALL_ACTIVE;
            LOG_DEBUG_FMT("Call %s: ACTIVE (connected)", callId.c_str());
            break;

        case OpalConnection::CallResponseDenied:
        case OpalConnection::CallResponseRejected:
            newState = CallState::RELEASE_COMPLETE;
            LOG_WARN_FMT("Call %s: DENIED/REJECTED", callId.c_str());
            break;

        default:
            LOG_DEBUG_FMT("Call %s: Unknown response %d", callId.c_str(), response);
            return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        callStates_[callId] = newState;
    }

    H323Event event;
    event.type = H323EventType::CALL_STATE_CHANGED;
    event.callId = callId;
    event.callState = newState;

    sendEvent(event);
}

void H323EventHandler::OnConnectionClearing(H323Connection* connection,
                                            const std::string& reason) {
    if (!connection) {
        return;
    }

    std::string callId = getCallId(connection);

    LOG_INFO_FMT("Call clearing: %s - %s", callId.c_str(), reason.c_str());

    {
        std::lock_guard<std::mutex> lock(mutex_);
        callStates_[callId] = CallState::RELEASE_COMPLETE;
    }

    H323Event event;
    event.type = H323EventType::CALL_CLEARED;
    event.callId = callId;
    event.errorText = reason;
    event.callState = CallState::RELEASE_COMPLETE;

    sendEvent(event);
}

void H323EventHandler::OnCallEstablished(H323Connection* connection) {
    if (!connection) {
        return;
    }

    std::string callId = getCallId(connection);

    LOG_INFO_FMT("Call established: %s", callId.c_str());

    {
        std::lock_guard<std::mutex> lock(mutex_);
        callStates_[callId] = CallState::CALL_ACTIVE;
    }

    H323Event event;
    event.type = H323EventType::CALL_ESTABLISHED;
    event.callId = callId;
    event.callState = CallState::CALL_ACTIVE;

    sendEvent(event);
}

void H323EventHandler::OnCallReleased(H323Connection* connection) {
    if (!connection) {
        return;
    }

    std::string callId = getCallId(connection);

    LOG_INFO_FMT("Call released: %s", callId.c_str());

    {
        std::lock_guard<std::mutex> lock(mutex_);
        callStates_.erase(callId);
    }

    H323Event event;
    event.type = H323EventType::CALL_CLEARED;
    event.callId = callId;
    event.callState = CallState::RELEASE_COMPLETE;

    sendEvent(event);
}

void H323EventHandler::OnGatekeeperRegistered(bool success) {
    LOG_INFO_FMT("Gatekeeper registration: %s", success ? "SUCCESS" : "FAILED");

    H323Event event;
    event.type = success ? H323EventType::GATEKEEPER_REGISTERED
                         : H323EventType::GATEKEEPER_REGISTRATION_FAILED;
    event.error = success ? H323Error::SUCCESS : H323Error::GATEKEEPER_REGISTRATION_FAILED;

    sendEvent(event);
}

void H323EventHandler::OnGatekeeperDiscovered(bool success, const std::string& gkAddress) {
    LOG_INFO_FMT("Gatekeeper discovery: %s (%s)",
                 success ? "SUCCESS" : "FAILED", gkAddress.c_str());

    H323Event event;
    event.type = success ? H323EventType::GATEKEEPER_DISCOVERED
                         : H323EventType::GATEKEEPER_DISCOVERY_FAILED;
    event.gatekeeperAddress = gkAddress;
    event.error = success ? H323Error::SUCCESS : H323Error::GATEKEEPER_DISCOVERY_FAILED;

    sendEvent(event);
}

void H323EventHandler::sendEvent(const H323Event& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (callback_) {
        callback_(event);
    }
}

std::string H323EventHandler::getCallId(H323Connection* connection) {
    if (!connection) {
        return "";
    }
    // This will be implemented with actual H323Plus integration
    // For now, return a placeholder
    return std::to_string(reinterpret_cast<uintptr_t>(connection));
}

std::string H323EventHandler::getRemoteAddress(H323Connection* connection) {
    if (!connection) {
        return "";
    }
    // This will be implemented with actual H323Plus integration
    return "unknown";
}

std::string H323EventHandler::getRemoteNumber(H323Connection* connection) {
    if (!connection) {
        return "";
    }
    // This will be implemented with actual H323Plus integration
    return "unknown";
}

} // namespace h323p
