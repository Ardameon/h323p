#include <CppUTest/TestHarness.h>
#include <fstream>
#include <cstdio>
#include "utils/logging.hpp"

TEST_GROUP(LoggingTest) {
    void setup() override {
    }
    void teardown() override {
        // Clean up test log file
        std::remove("test.log");
    }
};

// Test log level conversion
TEST(LoggingTest, LogLevelConversion) {
    CHECK(h323p::stringToLogLevel("debug") == h323p::LogLevel::DEBUG);
    CHECK(h323p::stringToLogLevel("info") == h323p::LogLevel::INFO);
    CHECK(h323p::stringToLogLevel("warn") == h323p::LogLevel::WARN);
    CHECK(h323p::stringToLogLevel("error") == h323p::LogLevel::ERROR);
    CHECK(h323p::stringToLogLevel("unknown") == h323p::LogLevel::INFO);  // Default
}

TEST(LoggingTest, LogLevelToString) {
    STRCMP_EQUAL("DEBUG", h323p::logLevelToString(h323p::LogLevel::DEBUG).c_str());
    STRCMP_EQUAL("INFO", h323p::logLevelToString(h323p::LogLevel::INFO).c_str());
    STRCMP_EQUAL("WARN", h323p::logLevelToString(h323p::LogLevel::WARN).c_str());
    STRCMP_EQUAL("ERROR", h323p::logLevelToString(h323p::LogLevel::ERROR).c_str());
}

// Test logger singleton
TEST(LoggingTest, LoggerSingleton) {
    auto& logger1 = h323p::Logger::instance();
    auto& logger2 = h323p::Logger::instance();
    CHECK(&logger1 == &logger2);  // Same instance
}

// Test logger initialization
TEST(LoggingTest, LoggerInit) {
    auto& logger = h323p::Logger::instance();
    logger.init(h323p::LogLevel::DEBUG, "", true);
    CHECK(logger.getLevel() == h323p::LogLevel::DEBUG);
}

// Test log level filtering
TEST(LoggingTest, LogLevelFiltering) {
    auto& logger = h323p::Logger::instance();
    
    // Set level to ERROR
    logger.init(h323p::LogLevel::ERROR, "", true);
    
    // DEBUG, INFO, WARN should be filtered out
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warn message");
    
    // ERROR should be logged (but we can't easily verify without capturing output)
    logger.error("Error message");
}

// Test file logging
TEST(LoggingTest, FileLogging) {
    auto& logger = h323p::Logger::instance();
    logger.init(h323p::LogLevel::DEBUG, "test.log", true);
    
    logger.info("Test message");
    logger.shutdown();
    
    // Check if file was created and contains the message
    std::ifstream file("test.log");
    CHECK(file.good());
    
    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        if (line.find("Test message") != std::string::npos) {
            found = true;
            break;
        }
    }
    CHECK(found);
}

// Test quiet mode
TEST(LoggingTest, QuietMode) {
    auto& logger = h323p::Logger::instance();
    logger.init(h323p::LogLevel::DEBUG, "", true);  // quiet = true
    
    // In quiet mode, no output should be produced
    // (we can't easily verify this in a test)
    logger.info("Quiet message");
}

// Test log message format
TEST(LoggingTest, LogMessageFormat) {
    auto& logger = h323p::Logger::instance();
    logger.init(h323p::LogLevel::DEBUG, "test.log", true);
    
    logger.error("Format test");
    logger.shutdown();
    
    std::ifstream file("test.log");
    std::string line;
    while (std::getline(file, line)) {
        // Check format: YYYY-MM-DD HH:MM:SS.mmm [LEVEL] message
        CHECK(line.find("[ERROR]") != std::string::npos);
        CHECK(line.find("Format test") != std::string::npos);
        break;
    }
}
