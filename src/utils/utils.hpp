#pragma once

#include <string>
#include <vector>
#include <optional>

namespace h323p {
namespace utils {

// String utilities
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);

// Address structure
struct Address {
    std::string host;
    int port = 0;
    std::string protocol = "tcp";

    bool isValid() const { return !host.empty() && port > 0; }
    std::string toString() const;
};

// Parse address from string (format: host:port or host:port/protocol)
std::optional<Address> parseAddress(const std::string& addr);

// File utilities
bool fileExists(const std::string& path);
std::string readFile(const std::string& path);
bool writeFile(const std::string& path, const std::string& content);

} // namespace utils
} // namespace h323p
