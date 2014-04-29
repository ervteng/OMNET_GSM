//-------------------------------------------------------------
// file: gsmsim.h
//        (part of gsmsim)
//-------------------------------------------------------------

#include "omnetpp.h"

// Defines for the call statistics
#define CALL_LENGTH_MIN 30
#define CALL_LENGTH 25
#define CALL_INTERVAL_MIN 5
#define CALL_INTERVAL 5
#define CALL_RETRY_LENGTH_MIN 5
#define CALL_RETRY_LENGTH 50
#define CALL_RETRY_INTERVAL_MIN 30
#define CALL_RETRY_INTERVAL 100

// Hard limit for handover
#define HANDOVER_LIMIT 0.05
#define WATT_MULTIPLY 200

// State of a phone
#define PHONE_STATE_CONNECTED 1
#define PHONE_STATE_DISCONNECTED 2

// message kind values (packet types):
enum {
    CONN_REQ,            // connection request                [MS->BTS]
    CONN_ACK,            // connection acknowledgement        [BTS->MS]
    CHECK_LINE,          // check Mobile Station              [MS->BTS]
    CHECK_BTS,         // check BTS                         [MS->BTS]
    CHECK_MS,            // check MS                          [BTS->MS]
    FORCE_CHECK_MS,      // check MS                          [BSC->BTS]
    FORCE_DISC,          // force disconnect                  [BTS->MS]
    TRIGGER,             // Trigger in MS                     [MS->MS]
    BTS_DATA,            // data from the BTS                 [BTS->MS]
    MS_DATA,             // data from the MS                  [MS->BTS]
    HANDOVER_CHK,        // check for handover                [BTS->BSC]
    HANDOVER_END,        // end of timelimit                  [BSC->BSC]
    HANDOVER_DATA,       // data for handover                 [BTS->BSC]
    HANDOVER_MS,         // handover MS, connect to new BTS   [BTS->MS]
    HANDOVER_BTS_DISC,   // handover BTS, disconnect MS       [BSC->BTS]
    DISC_REQ,            // disconnect request                [MS->BTS]
    DISC_ACK,            // disconnect acknowledgement        [BTS->MS]
    BTS_BEACON,           // Beacon sent by BTS                [BTS->MS]
    STOP_SCANNING,     // Self-message used by MS to determine when to stop listenting for beacons [MS]
    START_SCANNING
};
