#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

namespace h323p {

// Elapsed time timer
class ElapsedTimer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::milliseconds;

    ElapsedTimer() { start(); }

    // Start the timer
    void start();

    // Get elapsed time
    Duration elapsed() const;

    // Get elapsed time in seconds
    double elapsedSeconds() const;

    // Reset the timer
    void reset();

private:
    TimePoint startTime_;
};

// Countdown timer with callback
class CountdownTimer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::milliseconds;
    using Callback = std::function<void()>;

    CountdownTimer() = default;
    ~CountdownTimer();

    // Start the timer
    void start(Duration duration, Callback callback = nullptr);

    // Stop the timer
    void stop();

    // Reset the timer
    void reset();

    // Check if timer has expired
    bool isExpired() const;

    // Get remaining time
    Duration remaining() const;

    // Check if timer is running
    bool isRunning() const { return running_; }

private:
    TimePoint startTime_;
    Duration duration_;
    Callback callback_;
    std::atomic<bool> running_{false};
    std::thread thread_;
    mutable std::mutex mutex_;
};

} // namespace h323p
