#include "core/call_manager.hpp"
#include "core/call.hpp"
#include "utils/logging.hpp"

namespace h323p {

CallManager::CallManager() {
    LOG_DEBUG("CallManager created");
}

CallManager::~CallManager() {
    LOG_DEBUG("CallManager destroyed");
    stopAllCalls();
}

CallManager& CallManager::instance() {
    static CallManager manager;
    return manager;
}

void CallManager::setMaxCalls(size_t max) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    maxCalls_ = max;
    LOG_INFO_FMT("Max calls set to: %zu", max);
}

void CallManager::setRepeatCount(size_t count) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    repeatCount_ = count;
    LOG_INFO_FMT("Repeat count set to: %zu", count);
}

void CallManager::setCycleMode(bool enable) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    cycleMode_ = enable;
    LOG_INFO_FMT("Cycle mode: %s", enable ? "enabled" : "disabled");
}

void CallManager::setCallDurationRange(int minSeconds, int maxSeconds) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    minCallDuration_ = minSeconds;
    maxCallDuration_ = maxSeconds;
    LOG_INFO_FMT("Call duration range: %d-%d seconds", minSeconds, maxSeconds);
}

std::string CallManager::startOutgoingCall(const std::string& destination) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    // Check max calls limit
    if (activeCalls_.size() >= maxCalls_) {
        LOG_WARN_FMT("Cannot start call: max calls (%zu) reached", maxCalls_);
        return "";
    }
    
    // Create call ID
    std::string callId = "call-" + std::to_string(totalCalls_);
    
    // Create call object
    auto call = std::make_shared<Call>(callId, false);  // false = outgoing
    call->setup(destination);
    
    // Add to active calls
    activeCalls_.push_back(call);
    totalCalls_++;
    
    LOG_INFO_FMT("Outgoing call started: %s -> %s", callId.c_str(), destination.c_str());
    
    // Notify callback
    if (stateCallback_) {
        stateCallback_(callId, CallState::CALL_INITIATED);
    }
    
    return callId;
}

void CallManager::startListening() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    listening_ = true;
    LOG_INFO("Started listening for incoming calls");
}

void CallManager::stopListening() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    listening_ = false;
    LOG_INFO("Stopped listening for incoming calls");
}

void CallManager::stopAllCalls() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    LOG_INFO_FMT("Stopping all calls (%zu active)", activeCalls_.size());
    
    for (auto& call : activeCalls_) {
        if (call) {
            call->release(16);  // Normal call clearing
            
            // Move to completed
            completedCalls_.push_back(call);
            
            // Notify callback
            if (stateCallback_) {
                stateCallback_(call->getCallId(), CallState::RELEASE_COMPLETE);
            }
        }
    }
    
    activeCalls_.clear();
    listening_ = false;
    
    updateStats();
    
    LOG_INFO("All calls stopped");
}

void CallManager::addCall(std::shared_ptr<Call> call) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    if (!call) {
        LOG_ERROR("Attempted to add null call");
        return;
    }
    
    activeCalls_.push_back(call);
    totalCalls_++;
    
    LOG_INFO_FMT("Call added: %s (total active: %zu)",
                 call->getCallId().c_str(), activeCalls_.size());
}

void CallManager::removeCall(const std::string& callId) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    auto it = std::find_if(activeCalls_.begin(), activeCalls_.end(),
        [&callId](const std::shared_ptr<Call>& call) {
            return call->getCallId() == callId;
        });
    
    if (it != activeCalls_.end()) {
        // Move to completed
        completedCalls_.push_back(*it);
        activeCalls_.erase(it);
        
        LOG_INFO_FMT("Call removed: %s (active: %zu, completed: %zu)",
                     callId.c_str(), activeCalls_.size(), completedCalls_.size());
        
        updateStats();
    }
}

std::shared_ptr<Call> CallManager::getCall(const std::string& callId) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    // Search active calls
    for (auto& call : activeCalls_) {
        if (call->getCallId() == callId) {
            return call;
        }
    }
    
    // Search completed calls
    for (auto& call : completedCalls_) {
        if (call->getCallId() == callId) {
            return call;
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<Call>> CallManager::getActiveCalls() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return activeCalls_;
}

std::vector<std::shared_ptr<Call>> CallManager::getAllCalls() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    std::vector<std::shared_ptr<Call>> all;
    all.insert(all.end(), activeCalls_.begin(), activeCalls_.end());
    all.insert(all.end(), completedCalls_.begin(), completedCalls_.end());
    
    return all;
}

CallStats CallManager::getStats() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    CallStats stats;
    stats.totalCalls = totalCalls_;
    stats.activeCalls = activeCalls_.size();
    stats.completedCalls = completedCalls_.size();
    stats.failedCalls = failedCalls_;
    
    // Calculate durations
    std::chrono::milliseconds totalDuration{0};
    size_t completedCount = 0;
    
    for (const auto& call : completedCalls_) {
        if (call->getState() == CallState::RELEASE_COMPLETE) {
            totalDuration += call->getDuration();
            completedCount++;
        }
    }
    
    stats.totalDuration = totalDuration;
    if (completedCount > 0) {
        stats.averageDuration = totalDuration / completedCount;
    }
    
    return stats;
}

size_t CallManager::getActiveCallCount() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return activeCalls_.size();
}

void CallManager::setCallStateCallback(CallStateCallback callback) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    stateCallback_ = std::move(callback);
}

void CallManager::updateStats() {
    // Note: Caller must hold lock
    stats_ = getStats();
}

} // namespace h323p
