#pragma once

#include <string>
#include <vector>
#include <mutex>

#include "h323/h323_errors.hpp"

namespace h323p {

/**
 * Gatekeeper discovery result
 */
struct GatekeeperInfo {
    std::string address;
    std::string rasAddress;
    std::string callSignalAddress;
    int port = 1719;
};

/**
 * H.225 RAS Manager
 * 
 * Handles Gatekeeper discovery and registration (H.225 RAS protocol)
 * 
 * TODO: Implement in Stage 2
 */
class RASManager {
public:
    RASManager();
    ~RASManager();

    /**
     * Discover gatekeepers on the network
     * @param timeout Discovery timeout in milliseconds
     * @return List of discovered gatekeepers
     */
    std::vector<GatekeeperInfo> discoverGatekeepers(int timeout = 5000);

    /**
     * Register with a specific gatekeeper
     * @param gkAddress Gatekeeper address
     * @param username Username for registration
     * @param password Password for registration
     * @return Error code (SUCCESS if successful)
     */
    H323Error registerWithGatekeeper(const std::string& gkAddress,
                                      const std::string& username,
                                      const std::string& password);

    /**
     * Unregister from gatekeeper
     * @return Error code (SUCCESS if successful)
     */
    H323Error unregisterFromGatekeeper();

    /**
     * Check if registered with gatekeeper
     */
    bool isRegistered() const;

    /**
     * Get current gatekeeper address
     */
    std::string getGatekeeperAddress() const;

    /**
     * Refresh registration (send RAS RRQ)
     * @return Error code (SUCCESS if successful)
     */
    H323Error refreshRegistration();

private:
    std::string gkAddress_;
    bool registered_;
    mutable std::mutex mutex_;
};

} // namespace h323p
