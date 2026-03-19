#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <cstring>

#include "cli/cli_parser.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

using h323p::LogLevel;
using h323p::stringToLogLevel;

// Global shutdown flag
std::atomic<bool> g_shutdown_requested{false};

// Signal handler for graceful shutdown
void signalHandler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        g_shutdown_requested = true;
        LOG_INFO("Shutdown requested (signal " + std::to_string(signum) + ")...");
    }
}

// Execute call command (placeholder for Stage 2)
int executeCall(const h323p::CommandConfig& config) {
    LOG_INFO("Command: call");
    LOG_INFO("Destination: " + config.destination);
    
    // Placeholder: wait for shutdown or timeout
    LOG_INFO("Waiting for call to complete (placeholder - Stage 2 implementation pending)");
    
    int waitCount = 0;
    while (!g_shutdown_requested && waitCount < 5) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waitCount++;
    }
    
    if (g_shutdown_requested) {
        LOG_INFO("Call interrupted by user");
    }
    
    return 0;
}

// Execute listen command (placeholder for Stage 2)
int executeListen(const h323p::CommandConfig& config) {
    LOG_INFO("Command: listen");
    LOG_INFO("Max calls: " + std::to_string(config.maxCalls));
    
    // Placeholder: wait for shutdown
    LOG_INFO("Listening for incoming calls (placeholder - Stage 2 implementation pending)");
    
    while (!g_shutdown_requested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}

// Execute load command (placeholder for Stage 5)
int executeLoad(const h323p::CommandConfig& config) {
    LOG_INFO("Command: load");
    LOG_INFO("Destination: " + config.destination);
    LOG_INFO("Max calls: " + std::to_string(config.maxCalls));
    
    // Placeholder
    LOG_INFO("Load testing (placeholder - Stage 5 implementation pending)");
    
    while (!g_shutdown_requested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}

// Execute fuzz command (placeholder for Stage 6)
int executeFuzz(const h323p::CommandConfig& config) {
    LOG_INFO("Command: fuzz");
    LOG_INFO("Destination: " + config.destination);
    LOG_INFO("Type: " + config.fuzzType);
    
    // Placeholder
    LOG_INFO("Fuzzing (placeholder - Stage 6 implementation pending)");
    
    while (!g_shutdown_requested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}

// Execute register command (placeholder for Stage 2)
int executeRegister(const h323p::CommandConfig& config) {
    LOG_INFO("Command: register");
    LOG_INFO("Gatekeeper: " + config.gatekeeper);
    
    // Placeholder
    LOG_INFO("Registration (placeholder - Stage 2 implementation pending)");
    
    return 0;
}

// Execute info command
int executeInfo(const h323p::CommandConfig& config) {
    (void)config;  // Suppress unused parameter warning
    std::cout << "H.323 Test Utility (h323p) v" << PROJECT_VERSION << "\n";
    std::cout << "Based on H323Plus + PTLib\n";
    std::cout << "License: MPL-2.0\n";
    std::cout << "\n";
    std::cout << "Stage 1: Infrastructure (in progress)\n";
    std::cout << "Stage 2-7: Pending implementation\n";
    return 0;
}

int main(int argc, char* argv[]) {
    // Register signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Parse command-line arguments first (before logger init)
    h323p::CliParser parser;
    h323p::CommandConfig config = parser.parse(argc, argv);

    // Handle --version specially (show without logging)
    // Check if --version was in original args
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0) {
            std::cout << "H.323 Test Utility (h323p) v" << PROJECT_VERSION << std::endl;
            return 0;
        }
    }

    // Check for help or errors
    if (!parser.isValid()) {
        if (!parser.getError().empty()) {
            std::cerr << "Error: " << parser.getError() << std::endl;
        }
        std::cout << parser.getHelp() << std::endl;
        return parser.getError().empty() ? 0 : 1;
    }

    // Determine log level and file from config
    LogLevel logLevel = LogLevel::INFO;
    std::string logFile = config.logFile;
    bool quiet = config.quiet;

    // Override log level if specified
    if (!config.logLevel.empty()) {
        logLevel = stringToLogLevel(config.logLevel);
    }

    // Override quiet/verbose
    if (config.verbose) {
        logLevel = LogLevel::DEBUG;
    }
    if (config.quiet) {
        logLevel = LogLevel::NONE;
    }

    // Initialize logger once with proper configuration
    h323p::Logger::instance().init(logLevel, logFile, quiet);

    LOG_INFO("Starting h323p v" + std::string(PROJECT_VERSION));

    // Execute command
    int result = 0;

    switch (config.type) {
        case h323p::CommandType::CALL:
            result = executeCall(config);
            break;
        case h323p::CommandType::LISTEN:
            result = executeListen(config);
            break;
        case h323p::CommandType::LOAD:
            result = executeLoad(config);
            break;
        case h323p::CommandType::FUZZ:
            result = executeFuzz(config);
            break;
        case h323p::CommandType::REGISTER:
            result = executeRegister(config);
            break;
        case h323p::CommandType::INFO:
            result = executeInfo(config);
            break;
        default:
            std::cerr << "Unknown command" << std::endl;
            result = 1;
    }

    // Cleanup
    LOG_INFO("Cleaning up resources...");
    h323p::Logger::instance().shutdown();

    LOG_INFO("Goodbye!");
    return result;
}
