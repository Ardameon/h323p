#include <CppUTest/TestHarness.h>
#include "utils/utils.hpp"

TEST_GROUP(UtilsTest) {
    void setup() override {
    }
    void teardown() override {
    }
};

// Test trim function
TEST(UtilsTest, TrimWhitespace) {
    STRCMP_EQUAL("hello", h323p::utils::trim("  hello  ").c_str());
    STRCMP_EQUAL("hello", h323p::utils::trim("hello").c_str());
    STRCMP_EQUAL("", h323p::utils::trim("   ").c_str());
    STRCMP_EQUAL("", h323p::utils::trim("").c_str());
    STRCMP_EQUAL("hello world", h323p::utils::trim("\t\nhello world\r\n").c_str());
}

// Test split function
TEST(UtilsTest, SplitString) {
    auto result = h323p::utils::split("a,b,c", ',');
    LONGS_EQUAL(3, result.size());
    STRCMP_EQUAL("a", result[0].c_str());
    STRCMP_EQUAL("b", result[1].c_str());
    STRCMP_EQUAL("c", result[2].c_str());
}

TEST(UtilsTest, SplitStringEmpty) {
    auto result = h323p::utils::split("", ',');
    LONGS_EQUAL(1, result.size());
    STRCMP_EQUAL("", result[0].c_str());
}

TEST(UtilsTest, SplitStringSingle) {
    auto result = h323p::utils::split("single", ',');
    LONGS_EQUAL(1, result.size());
    STRCMP_EQUAL("single", result[0].c_str());
}

// Test join function
TEST(UtilsTest, JoinStrings) {
    std::vector<std::string> parts = {"a", "b", "c"};
    STRCMP_EQUAL("a,b,c", h323p::utils::join(parts, ",").c_str());
    STRCMP_EQUAL("a|b|c", h323p::utils::join(parts, "|").c_str());
}

TEST(UtilsTest, JoinStringsEmpty) {
    std::vector<std::string> parts;
    STRCMP_EQUAL("", h323p::utils::join(parts, ",").c_str());
}

TEST(UtilsTest, JoinStringsSingle) {
    std::vector<std::string> parts = {"single"};
    STRCMP_EQUAL("single", h323p::utils::join(parts, ",").c_str());
}

// Test toLower function
TEST(UtilsTest, ToLower) {
    STRCMP_EQUAL("hello", h323p::utils::toLower("HELLO").c_str());
    STRCMP_EQUAL("hello", h323p::utils::toLower("hello").c_str());
    STRCMP_EQUAL("hello", h323p::utils::toLower("HeLLo").c_str());
    STRCMP_EQUAL("", h323p::utils::toLower("").c_str());
}

// Test toUpper function
TEST(UtilsTest, ToUpper) {
    STRCMP_EQUAL("HELLO", h323p::utils::toUpper("HELLO").c_str());
    STRCMP_EQUAL("HELLO", h323p::utils::toUpper("hello").c_str());
    STRCMP_EQUAL("HELLO", h323p::utils::toUpper("HeLLo").c_str());
    STRCMP_EQUAL("", h323p::utils::toUpper("").c_str());
}

// Test startsWith function
TEST(UtilsTest, StartsWith) {
    CHECK(h323p::utils::startsWith("hello world", "hello"));
    CHECK(h323p::utils::startsWith("hello", "hello"));
    CHECK(h323p::utils::startsWith("hello", ""));
    CHECK(!h323p::utils::startsWith("hello", "world"));
    CHECK(!h323p::utils::startsWith("hi", "hello"));
}

// Test endsWith function
TEST(UtilsTest, EndsWith) {
    CHECK(h323p::utils::endsWith("hello world", "world"));
    CHECK(h323p::utils::endsWith("hello", "hello"));
    CHECK(h323p::utils::endsWith("hello", ""));
    CHECK(!h323p::utils::endsWith("hello", "world"));
    CHECK(!h323p::utils::endsWith("hi", "hello"));
}

// Test Address parsing
TEST(UtilsTest, ParseAddress) {
    auto addr = h323p::utils::parseAddress("192.168.1.1:1720");
    CHECK(addr.has_value());
    STRCMP_EQUAL("192.168.1.1", addr->host.c_str());
    LONGS_EQUAL(1720, addr->port);
    STRCMP_EQUAL("tcp", addr->protocol.c_str());
}

TEST(UtilsTest, ParseAddressWithProtocol) {
    auto addr = h323p::utils::parseAddress("192.168.1.1:1720/udp");
    CHECK(addr.has_value());
    STRCMP_EQUAL("192.168.1.1", addr->host.c_str());
    LONGS_EQUAL(1720, addr->port);
    STRCMP_EQUAL("udp", addr->protocol.c_str());
}

TEST(UtilsTest, ParseAddressInvalid) {
    CHECK(!h323p::utils::parseAddress("invalid").has_value());
    CHECK(!h323p::utils::parseAddress("192.168.1.1").has_value());
    CHECK(!h323p::utils::parseAddress(":1720").has_value());
    CHECK(!h323p::utils::parseAddress("192.168.1.1:abc").has_value());
}

TEST(UtilsTest, AddressToString) {
    h323p::utils::Address addr;
    addr.host = "192.168.1.1";
    addr.port = 1720;
    addr.protocol = "tcp";
    STRCMP_EQUAL("192.168.1.1:1720/tcp", addr.toString().c_str());
}
