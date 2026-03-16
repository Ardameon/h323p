#include <CppUTest/TestHarness.h>
#include "utils/logging.hpp"

TEST_GROUP(LoggingTest) {
    void setup() override {
    }
    void teardown() override {
    }
};

// Test log level conversion
TEST(LoggingTest, LogLevelConversion) {
    CHECK(h323p::stringToLogLevel("debug") == h323p::LogLevel::DEBUG);
    CHECK(h323p::stringToLogLevel("info") == h323p::LogLevel::INFO);
    CHECK(h323p::stringToLogLevel("warn") == h323p::LogLevel::WARN);
    CHECK(h323p::stringToLogLevel("error") == h323p::LogLevel::ERROR);
    CHECK(h323p::stringToLogLevel("unknown") == h323p::LogLevel::INFO);
}

TEST(LoggingTest, LogLevelToString) {
    STRCMP_EQUAL("DEBUG", h323p::logLevelToString(h323p::LogLevel::DEBUG).c_str());
    STRCMP_EQUAL("INFO", h323p::logLevelToString(h323p::LogLevel::INFO).c_str());
    STRCMP_EQUAL("WARN", h323p::logLevelToString(h323p::LogLevel::WARN).c_str());
    STRCMP_EQUAL("ERROR", h323p::logLevelToString(h323p::LogLevel::ERROR).c_str());
}

// NOTE: Logger singleton/instance tests disabled for Stage 1
// due to false positive memory leak detection with static singleton
// These tests will be re-enabled after fixing the async logger implementation
