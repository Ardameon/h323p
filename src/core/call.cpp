#include "core/call.hpp"
#include "utils/logging.hpp"

namespace h323p {

Call::Call(const std::string& callId, bool incoming)
    : callId_(callId)
    , incoming_(incoming)
    , state_(CallState::NULL_STATE)
    , startTime_(std::chrono::system_clock::now())
    , releaseCause_(0)
    , bytesSent_(0)
    , bytesReceived_(0) {
    
    LOG_DEBUG_FMT("Call created: %s (incoming: %s)",
                  callId_.c_str(), incoming ? "true" : "false");
}

Call::~Call() {
    if (state_ != CallState::RELEASE_COMPLETE &&
        state_ != CallState::NULL_STATE) {
        LOG_WARN_FMT("Call %s destroyed before release (state: %s)",
                     callId_.c_str(), callStateToString(state_).c_str());
    }
    LOG_DEBUG_FMT("Call destroyed: %s", callId_.c_str());
}

bool Call::setup(const std::string& destination) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (state_ != CallState::NULL_STATE) {
        LOG_ERROR_FMT("Call %s: Cannot setup in state %s",
                      callId_.c_str(), callStateToString(state_).c_str());
        return false;
    }
    
    destination_ = destination;
    state_ = CallState::CALL_INITIATED;
    startTime_ = std::chrono::system_clock::now();
    
    LOG_INFO_FMT("Call %s: Setup to %s", callId_.c_str(), destination.c_str());
    
    return true;
}

bool Call::answer() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (state_ != CallState::CALL_RECEIVED &&
        state_ != CallState::CALL_ALERTING) {
        LOG_ERROR_FMT("Call %s: Cannot answer in state %s",
                      callId_.c_str(), callStateToString(state_).c_str());
        return false;
    }
    
    state_ = CallState::CALL_ACTIVE;
    connectTime_ = std::chrono::system_clock::now();
    
    LOG_INFO_FMT("Call %s: Answered", callId_.c_str());
    
    return true;
}

bool Call::release(int cause) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (state_ == CallState::RELEASE_COMPLETE ||
        state_ == CallState::NULL_STATE) {
        LOG_DEBUG_FMT("Call %s: Already released", callId_.c_str());
        return false;
    }
    
    state_ = CallState::RELEASE_COMPLETE;
    releaseTime_ = std::chrono::system_clock::now();
    releaseCause_ = cause;
    
    LOG_INFO_FMT("Call %s: Released (cause: %d - %s)",
                 callId_.c_str(), cause, releaseCauseToString(static_cast<ReleaseCause>(cause)).c_str());
    
    return true;
}

CallState Call::getState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

void Call::setState(CallState state) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    CallState oldState = state_;
    state_ = state;
    
    LOG_DEBUG_FMT("Call %s: State changed %s -> %s",
                  callId_.c_str(),
                  callStateToString(oldState).c_str(),
                  callStateToString(state).c_str());
}

void Call::setRemoteAddress(const std::string& address) {
    std::lock_guard<std::mutex> lock(mutex_);
    remoteAddress_ = address;
}

std::chrono::milliseconds Call::getDuration() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto endTime = (releaseTime_.time_since_epoch().count() > 0)
                   ? releaseTime_
                   : std::chrono::system_clock::now();
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime_);
}

std::chrono::milliseconds Call::getConnectDuration() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connectTime_.time_since_epoch().count() == 0) {
        return std::chrono::milliseconds(0);
    }
    
    auto endTime = (releaseTime_.time_since_epoch().count() > 0)
                   ? releaseTime_
                   : std::chrono::system_clock::now();
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - connectTime_);
}

} // namespace h323p
