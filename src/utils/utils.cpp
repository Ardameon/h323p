#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>

namespace h323p {
namespace utils {

std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i > 0) result += delimiter;
        result += parts[i];
    }
    return result;
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool startsWith(const std::string& str, const std::string& prefix) {
    if (prefix.size() > str.size()) return false;
    return str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Address::toString() const {
    return host + ":" + std::to_string(port) + "/" + protocol;
}

std::optional<Address> parseAddress(const std::string& addr) {
    Address result;

    // Format: host:port or host:port/protocol
    auto colonPos = addr.find(':');
    if (colonPos == std::string::npos) {
        return std::nullopt;
    }

    result.host = addr.substr(0, colonPos);

    auto slashPos = addr.find('/', colonPos);
    std::string portStr;
    if (slashPos != std::string::npos) {
        portStr = addr.substr(colonPos + 1, slashPos - colonPos - 1);
        result.protocol = addr.substr(slashPos + 1);
    } else {
        portStr = addr.substr(colonPos + 1);
    }

    try {
        result.port = std::stoi(portStr);
    } catch (...) {
        return std::nullopt;
    }

    return result.isValid() ? std::optional<Address>(result) : std::nullopt;
}

bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file) return false;

    file << content;
    return file.good();
}

} // namespace utils
} // namespace h323p
