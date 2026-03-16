#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace h323p {

// Forward declarations
class Call;

// Call Manager - manages multiple H.323 calls
// TODO: Implement in Stage 2
class CallManager {
public:
    // Get singleton instance
    static CallManager& instance();

    // Configuration
    void setMaxCalls(size_t max);
    void setRepeatCount(size_t count);
    void setCycleMode(bool enable);

    // Call management
    std::string startOutgoingCall(const std::string& destination);
    void startListening();
    void stopAllCalls();

    // Statistics
    size_t getActiveCalls() const;
    size_t getTotalCalls() const;

private:
    CallManager();
    ~CallManager();

    CallManager(const CallManager&) = delete;
    CallManager& operator=(const CallManager&) = delete;

    std::vector<std::shared_ptr<Call>> activeCalls_;
    mutable std::recursive_mutex mutex_;
    size_t maxCalls_ = 10;
    size_t repeatCount_ = 1;
    bool cycleMode_ = false;
    size_t totalCalls_ = 0;
};

} // namespace h323p
