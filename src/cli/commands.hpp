#pragma once

#include <string>

namespace h323p {

// Command types
enum class CommandType {
    CALL,       // Initiate a call
    LISTEN,     // Listen mode for incoming calls
    LOAD,       // Load testing
    FUZZ,       // Fuzzing testing
    REGISTER,   // Register with Gatekeeper
    INFO,       // System information
    UNKNOWN     // Unknown command
};

// Command configuration structure
struct CommandConfig {
    // Command type
    CommandType type = CommandType::UNKNOWN;
    
    // Destination
    std::string destination;
    
    // Scenario
    std::string scenario;
    
    // Gatekeeper
    std::string gatekeeper;
    std::string username;
    std::string password;
    
    // Call settings
    size_t maxCalls = 10;
    size_t repeatCount = 1;
    bool cycleMode = false;
    
    // Media settings
    bool video = false;
    bool h239 = false;
    std::string audioCodec = "G.711";
    std::string videoCodec = "H.264";
    int mediaEncBits = 0;  // 0 = disabled, 128/192/256
    
    // Timing
    int tminCall = 10;
    int tmaxCall = 30;
    
    // Tracing
    int traceLevel = 0;
    std::string outputFile;
    std::string cdrFile;
    
    // TLS
    bool tls = false;
    std::string tlsCert;
    std::string tlsKey;
    std::string tlsCaFile;
    
    // Ports
    int tcpBase = 0;
    int udpBase = 0;
    int rtpBase = 0;
    
    // Fuzzing
    std::string fuzzType;
    int fuzzPercentage = 0;
    
    // Global options
    std::string configFile;
    std::string logFile;
    std::string logLevel = "info";
    bool quiet = false;
    bool verbose = false;
};

} // namespace h323p
