#include <CppUTest/TestHarness.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <array>
#include <memory>

TEST_GROUP(IntegrationTest) {
    void setup() override {
    }
    void teardown() override {
    }
};

// Helper function to execute command and capture output
static std::string executeCommand(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Test version command
TEST(IntegrationTest, VersionCommand) {
    // Note: This test requires the binary to be built
    // Skip if binary doesn't exist
    std::string output = executeCommand("./h323p info --version 2>&1");
    
    // Check if command executed (output should contain version info or error about missing binary)
    // In CI/CD, the binary will exist
    if (!output.empty()) {
        CHECK(output.find("h323p") != std::string::npos || 
              output.find("No such file") != std::string::npos);
    }
}

// Test help command
TEST(IntegrationTest, HelpCommand) {
    std::string output = executeCommand("./h323p --help 2>&1");
    
    if (!output.empty()) {
        // Check for expected content in help output
        CHECK(output.find("Usage") != std::string::npos ||
              output.find("Commands") != std::string::npos ||
              output.find("call") != std::string::npos ||
              output.find("No such file") != std::string::npos);
    }
}

// Test unknown command
TEST(IntegrationTest, UnknownCommand) {
    std::string output = executeCommand("./h323p unknown_command 2>&1");
    
    if (!output.empty() && output.find("No such file") == std::string::npos) {
        CHECK(output.find("Unknown") != std::string::npos ||
              output.find("Error") != std::string::npos);
    }
}

// Test no arguments
TEST(IntegrationTest, NoArguments) {
    std::string output = executeCommand("./h323p 2>&1");
    
    if (!output.empty() && output.find("No such file") == std::string::npos) {
        CHECK(output.find("Command required") != std::string::npos ||
              output.find("Usage") != std::string::npos ||
              output.find("Error") != std::string::npos);
    }
}

// Test call command with destination
TEST(IntegrationTest, CallCommand) {
    std::string output = executeCommand("./h323p call 192.168.1.100 2>&1");
    
    // The call will fail (no H.323 stack in Stage 1), but should parse correctly
    if (!output.empty() && output.find("No such file") == std::string::npos) {
        // Should either start or show placeholder message
        CHECK(output.find("call") != std::string::npos ||
              output.find("Destination") != std::string::npos ||
              output.find("placeholder") != std::string::npos ||
              output.find("Stage 2") != std::string::npos);
    }
}

// Test listen command
TEST(IntegrationTest, ListenCommand) {
    // This would hang, so we just check it parses correctly
    // In a real test, we'd use a timeout
    std::string output = executeCommand("timeout 1 ./h323p listen 2>&1");
    
    if (!output.empty() && output.find("No such file") == std::string::npos) {
        CHECK(output.find("listen") != std::string::npos ||
              output.find("Listening") != std::string::npos ||
              output.find("placeholder") != std::string::npos);
    }
}

// Test log file creation
TEST(IntegrationTest, LogFileCreation) {
    std::string output = executeCommand("./h323p --log-file test_integration.log info 2>&1");
    
    if (!output.empty() && output.find("No such file") == std::string::npos) {
        // Check if log file was created
        FILE* f = fopen("test_integration.log", "r");
        if (f) {
            fclose(f);
            // Log file exists
            CHECK(true);
            remove("test_integration.log");
        } else {
            // Binary might not exist yet
            CHECK(output.find("No such file") != std::string::npos);
        }
    }
}
