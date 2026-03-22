#include "h323/gatekeeper_manager.hpp"
#include "utils/logging.hpp"

namespace h323p {

GatekeeperManager::GatekeeperManager()
    : status_(RegistrationStatus::NOT_REGISTERED) {
    LOG_DEBUG("GatekeeperManager created");
}

GatekeeperManager::~GatekeeperManager() {
    LOG_DEBUG("GatekeeperManager destroyed");
}

void GatekeeperManager::setConfig(const GatekeeperConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
    LOG_DEBUG_FMT("Gatekeeper config set: %s:%d", config.address.c_str(), config.port);
}

H323Error GatekeeperManager::registerWithGatekeeper() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // TODO: Implement registration using RASManager
    LOG_INFO_FMT("Registering with Gatekeeper: %s:%d",
                 config_.address.c_str(), config_.port);
    
    status_ = RegistrationStatus::REGISTERED;
    return H323Error::SUCCESS;
}

H323Error GatekeeperManager::unregisterFromGatekeeper() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // TODO: Implement unregistration using RASManager
    LOG_INFO("Unregistering from Gatekeeper");
    
    status_ = RegistrationStatus::NOT_REGISTERED;
    return H323Error::SUCCESS;
}

RegistrationStatus GatekeeperManager::getStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

bool GatekeeperManager::isRegistered() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_ == RegistrationStatus::REGISTERED;
}

std::string GatekeeperManager::getGatekeeperAddress() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_.address;
}

} // namespace h323p
