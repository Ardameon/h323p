#include <CppUTest/TestHarness.h>
#include <thread>
#include <chrono>
#include <atomic>
#include "utils/timer.hpp"

TEST_GROUP(TimerTest) {
    void setup() override {
    }
    void teardown() override {
    }
};

// Test ElapsedTimer
TEST(TimerTest, ElapsedTimerBasic) {
    h323p::ElapsedTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto elapsed = timer.elapsed();
    CHECK(elapsed.count() >= 40);  // Allow some tolerance
    CHECK(elapsed.count() < 200);  // Should not be too long
}

TEST(TimerTest, ElapsedTimerSeconds) {
    h323p::ElapsedTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    auto elapsed = timer.elapsedSeconds();
    CHECK(elapsed >= 0.1);
    CHECK(elapsed < 1.0);
}

TEST(TimerTest, ElapsedTimerReset) {
    h323p::ElapsedTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto elapsed = timer.elapsed();
    CHECK(elapsed.count() >= 40);
    CHECK(elapsed.count() < 150);  // Should be close to 50ms after reset
}

// Test CountdownTimer
TEST(TimerTest, CountdownTimerExpiration) {
    std::atomic<bool> callbackCalled{false};
    
    h323p::CountdownTimer timer;
    timer.start(std::chrono::milliseconds(50), [&callbackCalled]() {
        callbackCalled = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CHECK(callbackCalled);
}

TEST(TimerTest, CountdownTimerStop) {
    std::atomic<bool> callbackCalled{false};
    
    h323p::CountdownTimer timer;
    timer.start(std::chrono::milliseconds(100), [&callbackCalled]() {
        callbackCalled = true;
    });
    
    timer.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    CHECK(!callbackCalled);  // Should not be called after stop
}

TEST(TimerTest, CountdownTimerReset) {
    std::atomic<bool> callbackCalled{false};
    
    h323p::CountdownTimer timer;
    timer.start(std::chrono::milliseconds(100), [&callbackCalled]() {
        callbackCalled = true;
    });
    
    timer.reset();
    CHECK(!timer.isRunning());
}

TEST(TimerTest, CountdownTimerIsRunning) {
    h323p::CountdownTimer timer;
    CHECK(!timer.isRunning());
    
    timer.start(std::chrono::milliseconds(500));
    CHECK(timer.isRunning());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    CHECK(!timer.isRunning());  // Should stop after expiration
}

TEST(TimerTest, CountdownTimerRemaining) {
    h323p::CountdownTimer timer;
    timer.start(std::chrono::milliseconds(200));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto remaining = timer.remaining();
    CHECK(remaining.count() > 0);
    CHECK(remaining.count() < 200);
}

TEST(TimerTest, CountdownTimerNoCallback) {
    h323p::CountdownTimer timer;
    timer.start(std::chrono::milliseconds(50), nullptr);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CHECK(timer.isExpired());
}
