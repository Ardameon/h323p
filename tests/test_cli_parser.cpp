#include <CppUTest/TestHarness.h>
#include "cli/cli_parser.hpp"

TEST_GROUP(CliParserTest) {
    void setup() override {
    }
    void teardown() override {
    }
};

// Test command parsing
TEST(CliParserTest, ParseCallCommand) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "call", "192.168.1.100"};
    auto config = parser.parse(3, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.type == h323p::CommandType::CALL);
    STRCMP_EQUAL("192.168.1.100", config.destination.c_str());
}

TEST(CliParserTest, ParseListenCommand) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "listen"};
    auto config = parser.parse(2, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.type == h323p::CommandType::LISTEN);
}

TEST(CliParserTest, ParseLoadCommand) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "load", "192.168.1.100", "-c", "100"};
    auto config = parser.parse(5, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.type == h323p::CommandType::LOAD);
    LONGS_EQUAL(100, config.maxCalls);
}

TEST(CliParserTest, ParseFuzzCommand) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "fuzz", "192.168.1.100", "-T", "rtp"};
    auto config = parser.parse(5, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.type == h323p::CommandType::FUZZ);
    STRCMP_EQUAL("rtp", config.fuzzType.c_str());
}

TEST(CliParserTest, ParseRegisterCommand) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "register", "-g", "192.168.1.1"};
    auto config = parser.parse(4, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.type == h323p::CommandType::REGISTER);
    STRCMP_EQUAL("192.168.1.1", config.gatekeeper.c_str());
}

TEST(CliParserTest, ParseInfoCommand) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "info"};
    auto config = parser.parse(2, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.type == h323p::CommandType::INFO);
}

// Test unknown command
TEST(CliParserTest, UnknownCommand) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "unknown"};
    auto config = parser.parse(2, const_cast<char**>(argv));
    
    CHECK(!parser.isValid());
    CHECK(config.type == h323p::CommandType::UNKNOWN);
}

// Test help option
TEST(CliParserTest, HelpOption) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "--help"};
    auto config = parser.parse(2, const_cast<char**>(argv));
    
    CHECK(!parser.isValid());
}

// Test global options
TEST(CliParserTest, GlobalOptions) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "--log-file", "test.log", "--log-level", "debug", "call", "192.168.1.100"};
    auto config = parser.parse(7, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    STRCMP_EQUAL("test.log", config.logFile.c_str());
    STRCMP_EQUAL("debug", config.logLevel.c_str());
}

TEST(CliParserTest, QuietOption) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "-q", "listen"};
    auto config = parser.parse(3, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.quiet);
}

TEST(CliParserTest, VerboseOption) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "--verbose", "listen"};
    auto config = parser.parse(3, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    CHECK(config.verbose);
}

// Test command-specific options
TEST(CliParserTest, CallOptions) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "call", "192.168.1.100", "-g", "192.168.1.1", "-u", "test"};
    auto config = parser.parse(7, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    STRCMP_EQUAL("192.168.1.100", config.destination.c_str());
    STRCMP_EQUAL("192.168.1.1", config.gatekeeper.c_str());
    STRCMP_EQUAL("test", config.username.c_str());
}

TEST(CliParserTest, ListenOptions) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "listen", "-m", "50"};
    auto config = parser.parse(4, const_cast<char**>(argv));
    
    CHECK(parser.isValid());
    LONGS_EQUAL(50, config.maxCalls);
}

// Test help output
TEST(CliParserTest, HelpOutput) {
    h323p::CliParser parser;
    std::string help = parser.getHelp();
    
    CHECK(help.find("h323p") != std::string::npos);
    CHECK(help.find("call") != std::string::npos);
    CHECK(help.find("listen") != std::string::npos);
    CHECK(help.find("load") != std::string::npos);
    CHECK(help.find("fuzz") != std::string::npos);
    CHECK(help.find("register") != std::string::npos);
    CHECK(help.find("info") != std::string::npos);
}

// Test missing destination for call
TEST(CliParserTest, CallMissingDestination) {
    h323p::CliParser parser;
    const char* argv[] = {"h323p", "call"};
    auto config = parser.parse(2, const_cast<char**>(argv));
    
    CHECK(!parser.isValid());
}
