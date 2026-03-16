#pragma once

#include "commands.hpp"
#include <string>

namespace h323p {

// Command-line parser
class CliParser {
public:
    CliParser();
    ~CliParser();

    // Parse command-line arguments
    CommandConfig parse(int argc, char* argv[]);

    // Check if parsing was successful
    bool isValid() const { return valid_; }

    // Get error message
    std::string getError() const { return error_; }

    // Get help text
    std::string getHelp() const;

private:
    bool valid_ = false;
    std::string error_;

    // Parse command type
    CommandType parseCommand(const std::string& cmd);

    // Validate configuration
    bool validateConfig(CommandConfig& config);

    // Parse global options
    void parseGlobalOptions(int argc, char* argv[], CommandConfig& config, int& i);

    // Parse command-specific options
    void parseCommandOptions(int argc, char* argv[], CommandConfig& config, int startIdx);
};

} // namespace h323p
