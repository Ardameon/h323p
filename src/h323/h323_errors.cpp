#include "h323_errors.hpp"

namespace h323p {

/**
 * Map H323Plus return codes to H323Error
 * 
 * H323Plus uses integer return codes where 0 typically means success
 * and positive values indicate various error conditions.
 * 
 * Note: This is a placeholder implementation. Actual mapping depends
 * on the specific H323Plus API being used.
 */
H323Error mapH323PlusCodeToError(int h323plusCode) {
    if (h323plusCode == 0) {
        return H323Error::SUCCESS;
    }
    
    // H323Plus specific error codes
    // These are examples - actual codes depend on H323Plus version
    switch (h323plusCode) {
        // Gatekeeper RAS errors (1-9)
        case 1: return H323Error::GATEKEEPER_DISCOVERY_FAILED;
        case 2: return H323Error::GATEKEEPER_REJECTED;
        case 3: return H323Error::GATEKEEPER_TIMEOUT;
        case 4: return H323Error::GATEKEEPER_ALREADY_REGISTERED;
        case 5: return H323Error::GATEKEEPER_NOT_REGISTERED;
        case 6: return H323Error::GATEKEEPER_REGISTRATION_FAILED;
        
        // Call errors (10-19)
        case 10: return H323Error::CALL_SETUP_FAILED;
        case 11: return H323Error::CALL_RELEASED_ABNORMALLY;
        case 12: return H323Error::CALL_CAPABILITY_MISMATCH;
        case 13: return H323Error::CALL_ALREADY_EXISTS;
        case 14: return H323Error::CALL_NOT_FOUND;
        case 15: return H323Error::CALL_TIMEOUT;
        case 16: return H323Error::CALL_REJECTED;
        
        // H.245 errors (20-29)
        case 20: return H323Error::H245_NOT_AVAILABLE;
        case 21: return H323Error::CAPABILITY_NEGOTIATION_FAILED;
        case 22: return H323Error::CHANNEL_OPEN_FAILED;
        case 23: return H323Error::MASTER_SLAVE_DETERMINATION_FAILED;
        case 24: return H323Error::H245_TIMEOUT;
        
        // Media errors (30-39)
        case 30: return H323Error::RTP_PORT_BIND_FAILED;
        case 31: return H323Error::RTCP_PORT_BIND_FAILED;
        case 32: return H323Error::CODEC_NOT_SUPPORTED;
        case 33: return H323Error::MEDIA_SEND_FAILED;
        case 34: return H323Error::MEDIA_RECEIVE_FAILED;
        case 35: return H323Error::JITTER_BUFFER_ERROR;
        
        // Network errors (40-49)
        case 40: return H323Error::NETWORK_UNREACHABLE;
        case 41: return H323Error::CONNECTION_TIMEOUT;
        case 42: return H323Error::CONNECTION_REFUSED;
        case 43: return H323Error::SOCKET_ERROR;
        case 44: return H323Error::DNS_RESOLUTION_FAILED;
        
        // Resource errors (50-59)
        case 50: return H323Error::OUT_OF_MEMORY;
        case 51: return H323Error::NO_AVAILABLE_PORTS;
        case 52: return H323Error::MAX_CALLS_REACHED;
        
        // Configuration errors (60-69)
        case 60: return H323Error::INVALID_CONFIGURATION;
        case 61: return H323Error::MISSING_PARAMETER;
        case 62: return H323Error::INVALID_PARAMETER;
        
        // Internal errors (90-99)
        case 90: return H323Error::INTERNAL_ERROR;
        case 91: return H323Error::NOT_IMPLEMENTED;
        case 92: return H323Error::INVALID_STATE;
        
        default:
            return H323Error::UNKNOWN_ERROR;
    }
}

/**
 * Map Q.931 release cause to H323Error
 */
H323Error mapQ931CauseToError(int q931Cause) {
    switch (q931Cause) {
        case 16: // Normal call clearing
            return H323Error::SUCCESS;
        
        case 17: // User busy
        case 18: // No user responding
        case 19: // No answer
            return H323Error::CALL_REJECTED;
        
        case 21: // Call rejected
            return H323Error::CALL_REJECTED;
        
        case 22: // Number changed
            return H323Error::INVALID_PARAMETER;
        
        case 28: // Invalid number format
            return H323Error::INVALID_PARAMETER;
        
        case 31: // Normal, unspecified
            return H323Error::SUCCESS;
        
        case 34: // No circuit/channel available
        case 38: // Network out of order
        case 41: // Temporary failure
        case 42: // Switching equipment congestion
            return H323Error::NETWORK_UNREACHABLE;
        
        case 47: // Service or option not available
            return H323Error::SERVICE_UNAVAILABLE;
        
        case 81: // Invalid call reference
        case 84: // Call does not exist
            return H323Error::CALL_NOT_FOUND;
        
        case 95: // Invalid message, unspecified
        case 96: // Information element non-existent
        case 97: // Invalid information element
        case 100: // Protocol error, unspecified
            return H323Error::PROTOCOL_ERROR;
        
        default:
            return H323Error::CALL_RELEASED_ABNORMALLY;
    }
}

} // namespace h323p
