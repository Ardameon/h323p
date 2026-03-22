#pragma once

#include <string>

namespace h323p {

/**
 * H323Plus error codes
 * 
 * Maps H323Plus library error codes to application-level errors
 */
enum class H323Error {
    // Success
    SUCCESS = 0,
    
    // Gatekeeper errors (H.225 RAS)
    GATEKEEPER_DISCOVERY_FAILED = 1,
    GATEKEEPER_REJECTED = 2,
    GATEKEEPER_TIMEOUT = 3,
    GATEKEEPER_ALREADY_REGISTERED = 4,
    GATEKEEPER_NOT_REGISTERED = 5,
    GATEKEEPER_REGISTRATION_FAILED = 6,
    
    // Call errors (H.225 Q.931)
    CALL_SETUP_FAILED = 10,
    CALL_RELEASED_ABNORMALLY = 11,
    CALL_CAPABILITY_MISMATCH = 12,
    CALL_ALREADY_EXISTS = 13,
    CALL_NOT_FOUND = 14,
    CALL_TIMEOUT = 15,
    CALL_REJECTED = 16,
    
    // H.245 errors
    H245_NOT_AVAILABLE = 20,
    CAPABILITY_NEGOTIATION_FAILED = 21,
    CHANNEL_OPEN_FAILED = 22,
    MASTER_SLAVE_DETERMINATION_FAILED = 23,
    H245_TIMEOUT = 24,
    
    // Media errors
    RTP_PORT_BIND_FAILED = 30,
    RTCP_PORT_BIND_FAILED = 31,
    CODEC_NOT_SUPPORTED = 32,
    MEDIA_SEND_FAILED = 33,
    MEDIA_RECEIVE_FAILED = 34,
    JITTER_BUFFER_ERROR = 35,
    
    // Network errors
    NETWORK_UNREACHABLE = 40,
    CONNECTION_TIMEOUT = 41,
    CONNECTION_REFUSED = 42,
    SOCKET_ERROR = 43,
    DNS_RESOLUTION_FAILED = 44,
    
    // Resource errors
    OUT_OF_MEMORY = 50,
    NO_AVAILABLE_PORTS = 51,
    MAX_CALLS_REACHED = 52,
    
    // Service errors
    SERVICE_UNAVAILABLE = 53,
    PROTOCOL_ERROR = 54,
    
    // Configuration errors
    INVALID_CONFIGURATION = 60,
    MISSING_PARAMETER = 61,
    INVALID_PARAMETER = 62,
    
    // Internal errors
    INTERNAL_ERROR = 90,
    NOT_IMPLEMENTED = 91,
    INVALID_STATE = 92,
    UNKNOWN_ERROR = 999
};

/**
 * Convert H323Error to string for logging
 */
inline std::string h323ErrorToString(H323Error error) {
    switch (error) {
        case H323Error::SUCCESS:
            return "Success";
        
        // Gatekeeper errors
        case H323Error::GATEKEEPER_DISCOVERY_FAILED:
            return "Gatekeeper Discovery Failed";
        case H323Error::GATEKEEPER_REJECTED:
            return "Gatekeeper Rejected";
        case H323Error::GATEKEEPER_TIMEOUT:
            return "Gatekeeper Timeout";
        case H323Error::GATEKEEPER_ALREADY_REGISTERED:
            return "Already Registered with Gatekeeper";
        case H323Error::GATEKEEPER_NOT_REGISTERED:
            return "Not Registered with Gatekeeper";
        case H323Error::GATEKEEPER_REGISTRATION_FAILED:
            return "Gatekeeper Registration Failed";
        
        // Call errors
        case H323Error::CALL_SETUP_FAILED:
            return "Call Setup Failed";
        case H323Error::CALL_RELEASED_ABNORMALLY:
            return "Call Released Abnormally";
        case H323Error::CALL_CAPABILITY_MISMATCH:
            return "Call Capability Mismatch";
        case H323Error::CALL_ALREADY_EXISTS:
            return "Call Already Exists";
        case H323Error::CALL_NOT_FOUND:
            return "Call Not Found";
        case H323Error::CALL_TIMEOUT:
            return "Call Timeout";
        case H323Error::CALL_REJECTED:
            return "Call Rejected";
        
        // H.245 errors
        case H323Error::H245_NOT_AVAILABLE:
            return "H.245 Not Available";
        case H323Error::CAPABILITY_NEGOTIATION_FAILED:
            return "Capability Negotiation Failed";
        case H323Error::CHANNEL_OPEN_FAILED:
            return "Channel Open Failed";
        case H323Error::MASTER_SLAVE_DETERMINATION_FAILED:
            return "Master/Slave Determination Failed";
        case H323Error::H245_TIMEOUT:
            return "H.245 Timeout";
        
        // Media errors
        case H323Error::RTP_PORT_BIND_FAILED:
            return "RTP Port Bind Failed";
        case H323Error::RTCP_PORT_BIND_FAILED:
            return "RTCP Port Bind Failed";
        case H323Error::CODEC_NOT_SUPPORTED:
            return "Codec Not Supported";
        case H323Error::MEDIA_SEND_FAILED:
            return "Media Send Failed";
        case H323Error::MEDIA_RECEIVE_FAILED:
            return "Media Receive Failed";
        case H323Error::JITTER_BUFFER_ERROR:
            return "Jitter Buffer Error";
        
        // Network errors
        case H323Error::NETWORK_UNREACHABLE:
            return "Network Unreachable";
        case H323Error::CONNECTION_TIMEOUT:
            return "Connection Timeout";
        case H323Error::CONNECTION_REFUSED:
            return "Connection Refused";
        case H323Error::SOCKET_ERROR:
            return "Socket Error";
        case H323Error::DNS_RESOLUTION_FAILED:
            return "DNS Resolution Failed";
        
        // Resource errors
        case H323Error::OUT_OF_MEMORY:
            return "Out of Memory";
        case H323Error::NO_AVAILABLE_PORTS:
            return "No Available Ports";
        case H323Error::MAX_CALLS_REACHED:
            return "Maximum Calls Reached";
        
        // Configuration errors
        case H323Error::INVALID_CONFIGURATION:
            return "Invalid Configuration";
        case H323Error::MISSING_PARAMETER:
            return "Missing Parameter";
        case H323Error::INVALID_PARAMETER:
            return "Invalid Parameter";
        
        // Internal errors
        case H323Error::INTERNAL_ERROR:
            return "Internal Error";
        case H323Error::NOT_IMPLEMENTED:
            return "Not Implemented";
        case H323Error::INVALID_STATE:
            return "Invalid State";
        case H323Error::UNKNOWN_ERROR:
            return "Unknown Error";
            
        default:
            return "Unknown Error Code";
    }
}

/**
 * Check if error is recoverable (can retry)
 */
inline bool isH323ErrorRecoverable(H323Error error) {
    switch (error) {
        case H323Error::GATEKEEPER_TIMEOUT:
        case H323Error::CALL_TIMEOUT:
        case H323Error::H245_TIMEOUT:
        case H323Error::CONNECTION_TIMEOUT:
        case H323Error::NETWORK_UNREACHABLE:
            return true;
        default:
            return false;
    }
}

/**
 * Get recommended action for error
 */
inline const char* getH323ErrorAction(H323Error error) {
    switch (error) {
        case H323Error::SUCCESS:
            return "Continue";
        
        case H323Error::GATEKEEPER_TIMEOUT:
        case H323Error::CALL_TIMEOUT:
        case H323Error::H245_TIMEOUT:
            return "Retry with backoff";
        
        case H323Error::GATEKEEPER_REJECTED:
        case H323Error::CALL_REJECTED:
            return "Check credentials/permissions";
        
        case H323Error::INVALID_CONFIGURATION:
        case H323Error::MISSING_PARAMETER:
        case H323Error::INVALID_PARAMETER:
            return "Fix configuration";
        
        case H323Error::CODEC_NOT_SUPPORTED:
            return "Check codec compatibility";
        
        case H323Error::MAX_CALLS_REACHED:
            return "Wait for calls to complete";
        
        default:
            return "Check logs for details";
    }
}

} // namespace h323p
