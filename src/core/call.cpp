#include "call.hpp"
#include "utils/logging.hpp"

namespace h323p {

Call::Call(const std::string& callId, bool incoming)
    : callId_(callId)
    , incoming_(incoming)
    , startTime_(std::chrono::system_clock::now())
{
    LOG_DEBUG("Call created: " + callId_ + " (incoming: " + std::to_string(incoming) + ")");
}

Call::~Call() {
    if (state_ != CallState::RELEASED) {
        release(0);
    }
    LOG_DEBUG("Call destroyed: " + callId_);
}

bool Call::setup(const std::string& destination) {
    // TODO: Implement actual call setup in Stage 2
    LOG_INFO("Call setup to: " + destination + " (placeholder)");
    state_ = CallState::SETUP;
    return true;
}

bool Call::answer() {
    // TODO: Implement actual answer in Stage 2
    LOG_INFO("Call answered: " + callId_ + " (placeholder)");
    state_ = CallState::CONNECTED;
    connectTime_ = std::chrono::system_clock::now();
    return true;
}

bool Call::release(int cause) {
    // TODO: Implement actual release in Stage 2
    LOG_INFO("Call released: " + callId_ + " (cause: " + std::to_string(cause) + ") (placeholder)");
    state_ = CallState::RELEASED;
    releaseTime_ = std::chrono::system_clock::now();
    return true;
}

} // namespace h323p
