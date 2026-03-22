#pragma once

#include <string>

namespace h323p {

/**
 * H.323 Call States
 * 
 * State machine for H.323 call lifecycle:
 * 
 * NULL → INITIATED → PROCEEDING → ALERTING → ACTIVE → RELEASED
 *                  ↓              ↓
 *               RELEASED     RELEASED
 * 
 * Based on Q.931 call states
 */
enum class CallState {
    NULL_STATE = 0,           // Initial state - no call
    CALL_INITIATED = 1,       // Outgoing call initiated (SETUP sent)
    CALL_RECEIVED = 2,        // Incoming call received (SETUP received)
    CALL_PRESENTED = 3,       // Call presented to user
    CALL_PROCEEDING = 4,      // Call proceeding (CALL PROCEEDING received)
    CALL_DELIVERED = 5,       // Call delivered to remote party
    CALL_ALERTING = 6,        // Remote party is alerting (ringing)
    CALL_ACTIVE = 7,          // Call is active (CONNECT received)
    RELEASE_REQUEST = 8,      // Release requested (RELEASE sent)
    RELEASE_INDICATION = 9,   // Release indicated (RELEASE received)
    RELEASE_COMPLETE = 10,    // Release complete (RELEASE COMPLETE received)
    ERROR_STATE = 11          // Error occurred
};

/**
 * Convert CallState to string for logging
 */
inline std::string callStateToString(CallState state) {
    switch (state) {
        case CallState::NULL_STATE: return "NULL";
        case CallState::CALL_INITIATED: return "INITIATED";
        case CallState::CALL_RECEIVED: return "RECEIVED";
        case CallState::CALL_PRESENTED: return "PRESENTED";
        case CallState::CALL_PROCEEDING: return "PROCEEDING";
        case CallState::CALL_DELIVERED: return "DELIVERED";
        case CallState::CALL_ALERTING: return "ALERTING";
        case CallState::CALL_ACTIVE: return "ACTIVE";
        case CallState::RELEASE_REQUEST: return "RELEASE_REQ";
        case CallState::RELEASE_INDICATION: return "RELEASE_IND";
        case CallState::RELEASE_COMPLETE: return "RELEASED";
        case CallState::ERROR_STATE: return "ERROR";
        default: return "UNKNOWN";
    }
}

/**
 * Release causes (Q.850 / Q.931)
 * 
 * Standard release causes for H.323 call termination
 */
enum class ReleaseCause {
    // Normal causes
    NORMAL_CALL_CLEARING = 16,           // Normal call clearing
    USER_BUSY = 17,                      // User busy
    NO_USER_RESPONDING = 18,             // No user responding
    NO_ANSWER = 19,                      // No answer (user alerted)
    CALL_ACCEPTED = 20,                  // Call accepted (used internally)
    CALL_REJECTED = 21,                  // Call rejected
    NUMBER_CHANGED = 22,                 // Number changed
    
    // Resource causes
    INVALID_NUMBER_FORMAT = 28,          // Invalid number format
    FACILITY_REJECTED = 29,              // Facility rejected
    RESPONSE_TO_STATUS_ENQUIRY = 30,     // Response to STATUS ENQUIRY
    NORMAL_UNSPECIFIED = 31,             // Normal, unspecified
    
    // Network causes
    NO_CIRCUIT_CHANNEL_AVAILABLE = 34,   // No circuit/channel available
    NETWORK_OUT_OF_ORDER = 38,           // Network out of order
    TEMPORARY_FAILURE = 41,              // Temporary failure
    SWITCHING_EQUIPMENT_CONGESTION = 42, // Switching equipment congestion
    ACCESS_INFORMATION_DISCARDED = 43,   // Access information discarded
    REQUESTED_CIRCUIT_NOT_AVAILABLE = 44, // Requested circuit/channel not available
    
    // Service/option causes
    SERVICE_UNAVAILABLE = 47,            // Service or option not available
    SERVICE_NOT_IMPLEMENTED = 63,        // Service or option not implemented
    
    // Invalid causes
    INVALID_CALL_REFERENCE = 81,         // Invalid call reference
    IDENTIFIED_CHANNEL_DOES_NOT_EXIST = 82, // Identified channel does not exist
    CALL_SUSPENDED = 83,                // Call suspended
    CALL_DOES_NOT_EXIST = 84,            // Call does not exist
    
    // Protocol errors
    INVALID_TRANSFERRED_NETWORK = 85,    // Invalid transferred network
    INVALID_MESSAGE_UNSPECIFIED = 95,    // Invalid message, unspecified
    INFORMATION_ELEMENT_NON_EXISTENT = 96, // Information element non-existent
    INVALID_INFORMATION_ELEMENT = 97,    // Invalid information element
    MESSAGE_INCOMPATIBLE = 98,           // Message not compatible with call state
    INFORMATION_ELEMENT_MANDATORY = 99,  // Mandatory information element missing
    PROTOCOL_ERROR = 100,                // Protocol error, unspecified
    INTERWORKING = 127                   // Interworking, unspecified
};

/**
 * Convert ReleaseCause to string for logging
 */
inline std::string releaseCauseToString(ReleaseCause cause) {
    switch (cause) {
        case ReleaseCause::NORMAL_CALL_CLEARING:
            return "Normal Call Clearing (16)";
        case ReleaseCause::USER_BUSY:
            return "User Busy (17)";
        case ReleaseCause::NO_USER_RESPONDING:
            return "No User Responding (18)";
        case ReleaseCause::NO_ANSWER:
            return "No Answer (19)";
        case ReleaseCause::CALL_REJECTED:
            return "Call Rejected (21)";
        case ReleaseCause::NUMBER_CHANGED:
            return "Number Changed (22)";
        case ReleaseCause::INVALID_NUMBER_FORMAT:
            return "Invalid Number Format (28)";
        case ReleaseCause::NETWORK_OUT_OF_ORDER:
            return "Network Out of Order (38)";
        case ReleaseCause::TEMPORARY_FAILURE:
            return "Temporary Failure (41)";
        case ReleaseCause::SERVICE_UNAVAILABLE:
            return "Service Unavailable (47)";
        case ReleaseCause::INVALID_CALL_REFERENCE:
            return "Invalid Call Reference (81)";
        case ReleaseCause::PROTOCOL_ERROR:
            return "Protocol Error (100)";
        case ReleaseCause::INTERWORKING:
            return "Interworking (127)";
        default:
            return "Unknown Cause (" + std::to_string(static_cast<int>(cause)) + ")";
    }
}

/**
 * Check if call state is active (can participate in media)
 */
inline bool isCallStateActive(CallState state) {
    return state == CallState::CALL_ACTIVE;
}

/**
 * Check if call state is terminal (call has ended)
 */
inline bool isCallStateTerminal(CallState state) {
    return state == CallState::RELEASE_COMPLETE || 
           state == CallState::ERROR_STATE;
}

/**
 * Check if call state is outgoing (we initiated the call)
 */
inline bool isCallStateOutgoing(CallState state) {
    return state == CallState::CALL_INITIATED ||
           state == CallState::CALL_PROCEEDING ||
           state == CallState::CALL_DELIVERED ||
           state == CallState::CALL_ALERTING;
}

/**
 * Check if call state is incoming (remote initiated the call)
 */
inline bool isCallStateIncoming(CallState state) {
    return state == CallState::CALL_RECEIVED ||
           state == CallState::CALL_PRESENTED;
}

} // namespace h323p
