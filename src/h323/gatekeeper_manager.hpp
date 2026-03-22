#pragma once

#include <string>
#include <mutex>

#include "h323/h323_errors.hpp"

namespace h323p {

/**
 * Gatekeeper configuration
 */
struct GatekeeperConfig {
    std::string address;
    int port = 1719;
    std::string username;
    std::string password;
    int ttl = 600;  // Registration time-to-live in seconds
    bool autoRefresh = true;
};

/**
 * Registration status
 */
enum class RegistrationStatus {
    NOT_REGISTERED = 0,
    REGISTERING = 1,
    REGISTERED = 2,
    REJECTED = 3,
    ERROR = 4
};

/**
 * Gatekeeper Manager
 * 
 * Manages gatekeeper registration lifecycle including auto-refresh
 * 
 * TODO: Implement in Stage 2
 */
class GatekeeperManager {
public:
    GatekeeperManager();
    ~GatekeeperManager();

    /**
     * Configure gatekeeper settings
     */
    void setConfig(const GatekeeperConfig& config);

    /**
     * Get current configuration
     */
    const GatekeeperConfig& getConfig() const { return config_; }

    /**
     * Register with gatekeeper
     * @return Error code (SUCCESS if successful)
     */
    H323Error registerWithGatekeeper();

    /**
     * Unregister from gatekeeper
     * @return Error code (SUCCESS if successful)
     */
    H323Error unregisterFromGatekeeper();

    /**
     * Get current registration status
     */
    RegistrationStatus getStatus() const;

    /**
     * Check if registered
     */
    bool isRegistered() const;

    /**
     * Get gatekeeper address
     */
    std::string getGatekeeperAddress() const;

private:
    GatekeeperConfig config_;
    RegistrationStatus status_;
    mutable std::mutex mutex_;
};

} // namespace h323p
