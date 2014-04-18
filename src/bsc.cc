//-------------------------------------------------------------
// file: bsc.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"
#include "bsc.h"
#include <omnetpp.h>

Define_Module(BSC);

BSC::BSC() : cSimpleModule() {}


void BSC::initialize()
{
    delay = 0.4;
    cPar& iPhones = par("phones");                   // Get the number of phones
    cPar& numbts = par("numbts");                    // Get the number of bts
    num_bts = numbts;
    iWatts = new double[iPhones.longValue()];                        // Allocate buffers
    iBTS = new int[iPhones.longValue()];                            // Allocate buffers
}


// End of simulation
void BSC::destroy() {
    // Release dynamic buffers
    if (iWatts)
        delete iWatts;
    if (iBTS)
        delete iBTS;
}

void BSC::handleMessage(cMessage *msg)
{
    int n = getNumParams();
    for (int i=0; i<n; i++) {
           cPar& p = par(i);
           ev << "parameter: " << p.getName() << "\n";
           ev << "  type:" << cPar::getTypeName(p.getType()) << "\n";
           ev << "  contains:" << p.str() << "\n";
    }

    int iType = msg->getKind();
    if (iType == HANDOVER_END){
        processHandoverEnd(msg);
    }else if (iType == HANDOVER_CHK){
        processHandoverCheck(msg);
    }else if (iType == HANDOVER_DATA){
        processHandoverData(msg);
    }else {
        EV << "No matched message type \n";
    }

}

void BSC::processHandoverEnd(cMessage *msg)
{
    int iMS = msg->par("ms");                    // MS identifier
    int i = msg->par("bts");                    // old BTS identifier
    cMessage *handover_bts_disc;

    if (iBTS[iMS] > -1)           // if there is a better BTS for the MS
            {
        if (iBTS[iMS] != i)    // not the original
                {
            // Send the handover message to the old BTS
            ev << " MS: " << iMS << " origBTS: " << i << " newBTS: "
                    << iBTS[iMS] << "\n";
            handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
                    HANDOVER_BTS_DISC);
            handover_bts_disc->addPar(*new cMsgPar("newbts")) = iBTS[iMS];
            handover_bts_disc->addPar(*new cMsgPar("src")) = 0;
            handover_bts_disc->addPar(*new cMsgPar("dest")) = i;
            handover_bts_disc->addPar(*new cMsgPar("ms")) = iMS;
            send(handover_bts_disc, "to_bts", i);
        }
    } else {
        // No BTS to hand over, send disconnect to BTS and MS
        handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
                HANDOVER_BTS_DISC);
        handover_bts_disc->addPar(*new cMsgPar("newbts")) = -1;
        handover_bts_disc->addPar(*new cMsgPar("src")) = 0;
        handover_bts_disc->addPar(*new cMsgPar("dest")) = i;
        handover_bts_disc->addPar(*new cMsgPar("ms")) = iMS;
        send(handover_bts_disc, "to_bts", i);
    }
}

void BSC::processHandoverCheck(cMessage *msg)
{
    bubble("HandoverCheck");
    int iClientAddr = msg->par("src");            // Old BTS
    int iMS = msg->par("ms");                    // MS
    iWatts[iMS] = msg->par("watt");            // Current watt
    cMessage *force_check_ms;
    simtime_t counter = simTime();

    if (iWatts[iMS] < 0)     // if current is below zero (no connection)
    {
        iBTS[iMS] = -1;             // Set the BTS identifier to invalid
    } else {
        iBTS[iMS] = iClientAddr;            // Set the old BTS number
    }

    int i;
    for (i = 0; i < num_bts; i++){      // Send check request to all BTSs
        if (i != iClientAddr) {
            force_check_ms = new cMessage("FORCE_CHECK_MS",
                    FORCE_CHECK_MS);
            force_check_ms->addPar(*new cMsgPar("dest")) = i;
            force_check_ms->addPar(*new cMsgPar("src")) = 0;
            force_check_ms->addPar(*new cMsgPar("ms")) = iMS;
            send(force_check_ms, "to_bts", i);
            ev << "Sending FORCE_CHECK_MS to BTS #" << i << "\n";
        };
    }
    // Send a scheduled message to myself to decide the handover
    cMessage *handover_end = new cMessage("HANDOVER_END", HANDOVER_END);
    handover_end->addPar(*new cMsgPar("ms")) = iMS;
    handover_end->addPar(*new cMsgPar("bts")) = iClientAddr;
    counter = simTime() + delay;
    scheduleAt(counter, handover_end);
}

void BSC::processHandoverData(cMessage *msg)
{
    int iClientAddr = msg->par("src");
    int iMS = msg->par("ms");
    double iWatt = msg->par("watt");
    ev << "got HANDOVER_DATA: Client: " << iClientAddr << " MS: " << iMS
            << " Watt: " << iWatt << " oldWatt:" << iWatts[iMS] << "\n";
    if ((iWatt > iWatts[iMS]) && (iWatt > 0)) // if it's better then the old then save it
            {
        iWatts[iMS] = iWatt;
        iBTS[iMS] = iClientAddr;
    }
}

//void BSC::activity() {
//    cMessage *msg, *force_check_ms, *handover_end, *handover_bts_disc;
//    int iType, iClientAddr = 0;
//    int iMS;
//    double delay = 0.4;
//    double iWatt;
//    int i;
//    simtime_t counter;
//
//    cPar& iPhones = par("phones");                   // Get the number of phones
//    cPar& numbts = par("numbts");                    // Get the number of bts
//    int num_bts = numbts;
//    iWatts = new double[iPhones.longValue()];                        // Allocate buffers
//    iBTS = new int[iPhones.longValue()];                            // Allocate buffers
//
//    counter = simTime();
//
//    for (;;) {
//        // receive msg
//        msg = receive();
//        iType = msg->getKind();
//        switch (iType) {
//
//        case HANDOVER_END:            // We must decide, which bts is the winner
//            iMS = msg->par("ms");                    // MS identifier
//            i = msg->par("bts");                    // old BTS identifier
//            if (iBTS[iMS] > -1)           // if there is a better BTS for the MS
//                    {
//                if (iBTS[iMS] != i)    // not the original
//                        {
//                    // Send the handover message to the old BTS
//                    ev << " MS: " << iMS << " origBTS: " << i << " newBTS: "
//                            << iBTS[iMS] << "\n";
//                    handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
//                            HANDOVER_BTS_DISC);
//                    handover_bts_disc->addPar(*new cMsgPar("newbts")) = iBTS[iMS];
//                    handover_bts_disc->addPar(*new cMsgPar("src")) = 0;
//                    handover_bts_disc->addPar(*new cMsgPar("dest")) = i;
//                    handover_bts_disc->addPar(*new cMsgPar("ms")) = iMS;
//                    send(handover_bts_disc, "to_bts", i);
//                }
//            } else {
//                // No BTS to hand over, send disconnect to BTS and MS
//                handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
//                        HANDOVER_BTS_DISC);
//                handover_bts_disc->addPar(*new cMsgPar("newbts")) = -1;
//                handover_bts_disc->addPar(*new cMsgPar("src")) = 0;
//                handover_bts_disc->addPar(*new cMsgPar("dest")) = i;
//                handover_bts_disc->addPar(*new cMsgPar("ms")) = iMS;
//                send(handover_bts_disc, "to_bts", i);
//            }
//            break;
//
//        case HANDOVER_CHK:              // Request to check the MS from the BTSs
//            iClientAddr = msg->par("src");            // Old BTS
//            iMS = msg->par("ms");                    // MS
//            iWatts[iMS] = msg->par("watt");            // Current watt
//            if (iWatts[iMS] < 0)     // if current is below zero (no connection)
//                    {
//                iBTS[iMS] = -1;             // Set the BTS identifier to invalid
//            } else {
//                iBTS[iMS] = iClientAddr;            // Set the old BTS number
//            }
//            for (i = 0; i < num_bts; i++)      // Send check request to all BTSs
//                    {
//                if (i != iClientAddr) {
//                    force_check_ms = new cMessage("FORCE_CHECK_MS",
//                            FORCE_CHECK_MS);
//                    force_check_ms->addPar(*new cMsgPar("dest")) = i;
//                    force_check_ms->addPar(*new cMsgPar("src")) = 0;
//                    force_check_ms->addPar(*new cMsgPar("ms")) = iMS;
//                    send(force_check_ms, "to_bts", i);
//                    ev << "Sending FORCE_CHECK_MS to BTS #" << i << "\n";
//                };
//            }
//            // Send a scheduled message to myself to decide the handover
//            handover_end = new cMessage("HANDOVER_END", HANDOVER_END);
//            handover_end->addPar(*new cMsgPar("ms")) = iMS;
//            handover_end->addPar(*new cMsgPar("bts")) = iClientAddr;
//            counter = simTime() + delay;
//            scheduleAt(counter, handover_end);
//            break;
//
//        case HANDOVER_DATA:                            // Watt data from BTS
//            iClientAddr = msg->par("src");
//            iMS = msg->par("ms");
//            iWatt = msg->par("watt");
//            ev << "got HANDOVER_DATA: Client: " << iClientAddr << " MS: " << iMS
//                    << " Watt: " << iWatt << " oldWatt:" << iWatts[iMS] << "\n";
//            if ((iWatt > iWatts[iMS]) && (iWatt > 0)) // if it's better then the old then save it
//                    {
//                iWatts[iMS] = iWatt;
//                iBTS[iMS] = iClientAddr;
//            }
//            break;
//
//        }
//    }
//}

