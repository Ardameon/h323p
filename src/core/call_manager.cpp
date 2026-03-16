#include "call_manager.hpp"
#include "call.hpp"
#include "utils/logging.hpp"

namespace h323p {

CallManager::CallManager() = default;

CallManager::~CallManager() {
    stopAllCalls();
}

CallManager& CallManager::instance() {
    static CallManager manager;
    return manager;
}

void CallManager::setMaxCalls(size_t max) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    maxCalls_ = max;
}

void CallManager::setRepeatCount(size_t count) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    repeatCount_ = count;
}

void CallManager::setCycleMode(bool enable) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    cycleMode_ = enable;
}

std::string CallManager::startOutgoingCall(const std::string& destination) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    if (activeCalls_.size() >= maxCalls_) {
        LOG_ERROR("Maximum number of calls reached");
        return "";
    }
    
    // TODO: Implement actual call creation in Stage 2
    LOG_INFO("Starting outgoing call to: " + destination + " (placeholder)");
    
    totalCalls_++;
    return "call-" + std::to_string(totalCalls_);
}

void CallManager::startListening() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    // TODO: Implement listening mode in Stage 2
    LOG_INFO("Starting listening mode (placeholder)");
}

void CallManager::stopAllCalls() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    // TODO: Implement call termination in Stage 2
    activeCalls_.clear();
    LOG_INFO("All calls stopped");
}

size_t CallManager::getActiveCalls() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return activeCalls_.size();
}

size_t CallManager::getTotalCalls() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return totalCalls_;
}

} // namespace h323p
