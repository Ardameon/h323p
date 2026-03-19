#include "cli_parser.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>

namespace h323p {

CliParser::CliParser() = default;
CliParser::~CliParser() = default;

CommandType CliParser::parseCommand(const std::string& cmd) {
    if (cmd == "call") return CommandType::CALL;
    if (cmd == "listen") return CommandType::LISTEN;
    if (cmd == "load") return CommandType::LOAD;
    if (cmd == "fuzz") return CommandType::FUZZ;
    if (cmd == "register") return CommandType::REGISTER;
    if (cmd == "info") return CommandType::INFO;
    return CommandType::UNKNOWN;
}

void CliParser::parseGlobalOptions(int argc, char* argv[], CommandConfig& config, int& i) {
    if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
        config.configFile = argv[++i];
    } else if (strcmp(argv[i], "--log-file") == 0 && i + 1 < argc) {
        config.logFile = argv[++i];
    } else if (strcmp(argv[i], "--log-level") == 0 && i + 1 < argc) {
        config.logLevel = argv[++i];
    } else if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) {
        config.quiet = true;
    } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
        config.verbose = true;
    }
}

void CliParser::parseCommandOptions(int argc, char* argv[], CommandConfig& config, int startIdx) {
    for (int j = startIdx; j < argc; j++) {
        // Also check for global options in command-specific parsing
        if (strcmp(argv[j], "--log-file") == 0 && j + 1 < argc) {
            config.logFile = argv[++j];
        } else if (strcmp(argv[j], "--log-level") == 0 && j + 1 < argc) {
            config.logLevel = argv[++j];
        } else if (strcmp(argv[j], "--quiet") == 0 || strcmp(argv[j], "-q") == 0) {
            config.quiet = true;
        } else if (strcmp(argv[j], "--verbose") == 0 || strcmp(argv[j], "-v") == 0) {
            config.verbose = true;
        } else if (strcmp(argv[j], "--config") == 0 && j + 1 < argc) {
            config.configFile = argv[++j];
        } else if (strcmp(argv[j], "--scenario") == 0 || strcmp(argv[j], "-s") == 0) {
            if (j + 1 < argc) config.scenario = argv[++j];
        } else if (strcmp(argv[j], "--gatekeeper") == 0 || strcmp(argv[j], "-g") == 0) {
            if (j + 1 < argc) config.gatekeeper = argv[++j];
        } else if (strcmp(argv[j], "--username") == 0 || strcmp(argv[j], "-u") == 0) {
            if (j + 1 < argc) config.username = argv[++j];
        } else if (strcmp(argv[j], "--password") == 0 || strcmp(argv[j], "-p") == 0) {
            if (j + 1 < argc) config.password = argv[++j];
        } else if (strcmp(argv[j], "--max-calls") == 0 || strcmp(argv[j], "-m") == 0) {
            if (j + 1 < argc) config.maxCalls = std::stoul(argv[++j]);
        } else if (strcmp(argv[j], "--repeat") == 0 || strcmp(argv[j], "-r") == 0) {
            if (j + 1 < argc) config.repeatCount = std::stoul(argv[++j]);
        } else if (strcmp(argv[j], "--cycle") == 0 || strcmp(argv[j], "-C") == 0) {
            config.cycleMode = true;
        } else if (strcmp(argv[j], "--video") == 0 || strcmp(argv[j], "-V") == 0) {
            config.video = true;
        } else if (strcmp(argv[j], "--h239") == 0) {
            config.h239 = true;
        } else if (strcmp(argv[j], "--audio-codec") == 0 || strcmp(argv[j], "-P") == 0) {
            if (j + 1 < argc) config.audioCodec = argv[++j];
        } else if (strcmp(argv[j], "--video-codec") == 0) {
            if (j + 1 < argc) config.videoCodec = argv[++j];
        } else if (strcmp(argv[j], "--media-enc") == 0) {
            if (j + 1 < argc) config.mediaEncBits = std::stoi(argv[++j]);
        } else if (strcmp(argv[j], "--tls") == 0) {
            config.tls = true;
        } else if (strcmp(argv[j], "--tls-cert") == 0) {
            if (j + 1 < argc) config.tlsCert = argv[++j];
        } else if (strcmp(argv[j], "--tls-key") == 0) {
            if (j + 1 < argc) config.tlsKey = argv[++j];
        } else if (strcmp(argv[j], "--tls-ca") == 0) {
            if (j + 1 < argc) config.tlsCaFile = argv[++j];
        } else if (strcmp(argv[j], "--tmin-call") == 0) {
            if (j + 1 < argc) config.tminCall = std::stoi(argv[++j]);
        } else if (strcmp(argv[j], "--tmax-call") == 0) {
            if (j + 1 < argc) config.tmaxCall = std::stoi(argv[++j]);
        } else if (strcmp(argv[j], "--cdr") == 0) {
            if (j + 1 < argc) config.cdrFile = argv[++j];
        } else if (strcmp(argv[j], "--output") == 0 || strcmp(argv[j], "-o") == 0) {
            if (j + 1 < argc) config.outputFile = argv[++j];
        } else if (strcmp(argv[j], "--trace") == 0 || strcmp(argv[j], "-t") == 0) {
            if (j + 1 < argc) config.traceLevel = std::stoi(argv[++j]);
        } else if (strcmp(argv[j], "--type") == 0 || strcmp(argv[j], "-T") == 0) {
            if (j + 1 < argc) config.fuzzType = argv[++j];
        } else if (strcmp(argv[j], "--percentage") == 0 || strcmp(argv[j], "-p") == 0) {
            if (j + 1 < argc) config.fuzzPercentage = std::stoi(argv[++j]);
        } else if (strcmp(argv[j], "--calls") == 0 || strcmp(argv[j], "-c") == 0) {
            if (j + 1 < argc) config.maxCalls = std::stoul(argv[++j]);
        } else if (strcmp(argv[j], "--rate") == 0) {
            // Rate option for load testing - stored but not used in Stage 1
            if (j + 1 < argc) { ++j; }
        } else if (strcmp(argv[j], "--duration") == 0 || strcmp(argv[j], "-d") == 0) {
            // Duration option - stored but not used in Stage 1
            if (j + 1 < argc) { ++j; }
        } else if (strcmp(argv[j], "--version") == 0) {
            // Version flag - will be handled in main
            config.type = CommandType::INFO;
            valid_ = false;  // Signal to show version only
            return;
        } else if (strcmp(argv[j], "--help") == 0 || strcmp(argv[j], "-h") == 0) {
            valid_ = false;
            return;
        } else if (argv[j][0] != '-' && config.type == CommandType::CALL) {
            config.destination = argv[j];
        }
    }
}

CommandConfig CliParser::parse(int argc, char* argv[]) {
    CommandConfig config;

    if (argc < 2) {
        error_ = "Command required. Use './h323p --help' for usage information.";
        return config;
    }

    bool versionRequested = false;

    // First pass: parse global options and find command
    for (int i = 1; i < argc; i++) {
        // Check for --version anywhere in the command line
        if (strcmp(argv[i], "--version") == 0) {
            versionRequested = true;
        }
        
        // Global options (before command)
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (strcmp(argv[i], "--config") == 0 || 
                strcmp(argv[i], "--log-file") == 0 ||
                strcmp(argv[i], "--log-level") == 0 ||
                strcmp(argv[i], "--quiet") == 0 ||
                strcmp(argv[i], "--verbose") == 0) {
                parseGlobalOptions(argc, argv, config, i);
            }
        } else if (argv[i][0] == '-' && argv[i][1] != '\0') {
            // Short options
            if (strcmp(argv[i], "-q") == 0 || 
                strcmp(argv[i], "-v") == 0) {
                parseGlobalOptions(argc, argv, config, i);
            }
        } else if (argv[i][0] != '-') {
            // Command found
            config.type = parseCommand(argv[i]);
            if (config.type == CommandType::UNKNOWN && 
                strcmp(argv[i], "--help") != 0 && 
                strcmp(argv[i], "-h") != 0) {
                error_ = "Unknown command '" + std::string(argv[i]) + "'. Use './h323p --help' for usage information.";
                return config;
            }
            // Parse command-specific options
            parseCommandOptions(argc, argv, config, i + 1);
            break;
        }
    }

    // Handle --version flag
    if (versionRequested) {
        config.type = CommandType::INFO;
        valid_ = false;  // Signal to show version only (no logging)
        return config;
    }

    // Validate configuration
    valid_ = (config.type != CommandType::UNKNOWN);
    if (valid_) {
        valid_ = validateConfig(config);
    }

    return config;
}

bool CliParser::validateConfig(CommandConfig& config) {
    // Check required parameters for CALL command
    if (config.type == CommandType::CALL && config.destination.empty()) {
        error_ = "Destination address required for call command";
        return false;
    }

    // Check ranges
    if (config.maxCalls == 0) {
        error_ = "maxCalls must be greater than 0";
        return false;
    }

    return true;
}

std::string CliParser::getHelp() const {
    std::ostringstream oss;
    oss << "H.323 Test Utility (h323p) v" << PROJECT_VERSION << "\n\n"
        << "Usage:\n"
        << "  h323p <command> [options]\n\n"
        << "Commands:\n"
        << "  call       Initiate an H.323 call\n"
        << "  listen     Listen mode for incoming calls\n"
        << "  load       Load testing\n"
        << "  fuzz       Fuzzing testing\n"
        << "  register   Register with Gatekeeper\n"
        << "  info       System information\n\n"
        << "Global Options:\n"
        << "  --config FILE      Configuration file\n"
        << "  --log-file FILE    Log file path\n"
        << "  --log-level LEVEL  Log level (debug/info/warn/error)\n"
        << "  --quiet, -q        Quiet mode (no output)\n"
        << "  --verbose, -v      Verbose mode\n"
        << "  --help, -h         Show this help\n\n"
        << "Examples:\n"
        << "  h323p call 192.168.1.100\n"
        << "  h323p load 192.168.1.100 -c 100 -r 10\n"
        << "  h323p listen -m 50\n"
        << "  h323p info --version\n";
    return oss.str();
}

} // namespace h323p
