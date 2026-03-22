#pragma once

#include "utils/logging.hpp"

// Forward declaration H323Plus classes
class H323Connection;

namespace h323p {

/**
 * Q.931 trace levels
 */
enum class Q931TraceLevel {
    NONE = 0,       // No logging
    BASIC = 1,      // Message types only
    DETAILS = 2,    // Main information elements
    FULL = 3        // All information elements
};

/**
 * Q.931 Message Logger
 * 
 * Logs Q.931 signaling messages for debugging
 * 
 * TODO: Implement in Stage 2
 */
class Q931Logger {
public:
    /**
     * Log SETUP message
     */
    static void logSetupMessage(const H323Connection* connection,
                                const char* direction,
                                Q931TraceLevel level);

    /**
     * Log CALL PROCEEDING message
     */
    static void logCallProceedingMessage(const H323Connection* connection,
                                         const char* direction,
                                         Q931TraceLevel level);

    /**
     * Log ALERTING message
     */
    static void logAlertingMessage(const H323Connection* connection,
                                   const char* direction,
                                   Q931TraceLevel level);

    /**
     * Log CONNECT message
     */
    static void logConnectMessage(const H323Connection* connection,
                                  const char* direction,
                                  Q931TraceLevel level);

    /**
     * Log RELEASE COMPLETE message
     */
    static void logReleaseCompleteMessage(const H323Connection* connection,
                                          const char* direction,
                                          Q931TraceLevel level);

private:
    static void logCallReferenceValue(const H323Connection* connection);
    static void logMessageType(const H323Connection* connection);
    static void logCallingPartyNumber(const H323Connection* connection);
    static void logCalledPartyNumber(const H323Connection* connection);
    static void logBearerCapability(const H323Connection* connection);
};

} // namespace h323p
