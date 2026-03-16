#include "timer.hpp"

namespace h323p {

// ElapsedTimer implementation

void ElapsedTimer::start() {
    startTime_ = Clock::now();
}

ElapsedTimer::Duration ElapsedTimer::elapsed() const {
    return std::chrono::duration_cast<Duration>(Clock::now() - startTime_);
}

double ElapsedTimer::elapsedSeconds() const {
    return std::chrono::duration<double>(elapsed()).count();
}

void ElapsedTimer::reset() {
    start();
}

// CountdownTimer implementation

CountdownTimer::~CountdownTimer() {
    stop();
}

void CountdownTimer::start(Duration duration, Callback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    stop();
    
    duration_ = duration;
    callback_ = callback;
    startTime_ = Clock::now();
    running_ = true;

    if (callback_) {
        thread_ = std::thread([this]() {
            std::this_thread::sleep_for(duration_);
            std::lock_guard<std::mutex> lock(mutex_);
            if (running_ && callback_) {
                callback_();
            }
        });
    }
}

void CountdownTimer::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

void CountdownTimer::reset() {
    stop();
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

bool CountdownTimer::isExpired() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) return false;
    auto elapsed = std::chrono::duration_cast<Duration>(
        Clock::now() - startTime_);
    return elapsed >= duration_;
}

CountdownTimer::Duration CountdownTimer::remaining() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) return Duration(0);
    auto elapsed = std::chrono::duration_cast<Duration>(
        Clock::now() - startTime_);
    auto rem = duration_ - elapsed;
    return rem.count() > 0 ? rem : Duration(0);
}

} // namespace h323p
