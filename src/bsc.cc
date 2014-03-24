//-------------------------------------------------------------
// file: bsc.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"

// The BSC object
class BSC: public cSimpleModule {
    Module_Class_Members(BSC,cSimpleModule,16384)
    virtual void activity();
    virtual void destroy();
    double *iWatts;              // Buffer to hold the max. watts for the phones
    int *iBTS;                          // Buffer to hold the bts for the phones
};

Define_Module(BSC);

// End of simulation
void BSC::destroy() {
    // Release dynamic buffers
    if (iWatts)
        delete iWatts;
    if (iBTS)
        delete iBTS;
}

void BSC::activity() {
    cMessage *msg, *force_check_ms, *handover_end, *handover_bts_disc;
    int iType, iClientAddr = 0;
    int iMS;
    double delay = 0.4;
    double iWatt;
    int i;
    simtime_t counter;

    cPar& iPhones = par("phones");                   // Get the number of phones
    cPar& numbts = par("numbts");                    // Get the number of bts
    int num_bts = numbts;
    iWatts = new double[iPhones];                        // Allocate buffers
    iBTS = new int[iPhones];                            // Allocate buffers

    counter = simTime();

    for (;;) {
        // receive msg
        msg = receive();
        iType = msg->kind();
        switch (iType) {

        case HANDOVER_END:            // We must decide, which bts is the winner
            iMS = msg->par("ms");                    // MS identifier
            i = msg->par("bts");                    // old BTS identifier
            if (iBTS[iMS] > -1)           // if there is a better BTS for the MS
                    {
                if (iBTS[iMS] != i)    // not the original
                        {
                    // Send the handover message to the old BTS
                    ev << " MS: " << iMS << " origBTS: " << i << " newBTS: "
                            << iBTS[iMS] << "\n";
                    handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
                            HANDOVER_BTS_DISC);
                    handover_bts_disc->addPar(*new cPar("newbts")) = iBTS[iMS];
                    handover_bts_disc->addPar(*new cPar("src")) = 0;
                    handover_bts_disc->addPar(*new cPar("dest")) = i;
                    handover_bts_disc->addPar(*new cPar("ms")) = iMS;
                    send(handover_bts_disc, "to_bts", i);
                }
            } else {
                // No BTS to hand over, send disconnect to BTS and MS
                handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
                        HANDOVER_BTS_DISC);
                handover_bts_disc->addPar(*new cPar("newbts")) = -1;
                handover_bts_disc->addPar(*new cPar("src")) = 0;
                handover_bts_disc->addPar(*new cPar("dest")) = i;
                handover_bts_disc->addPar(*new cPar("ms")) = iMS;
                send(handover_bts_disc, "to_bts", i);
            }
            break;

        case HANDOVER_CHK:              // Request to check the MS from the BTSs
            iClientAddr = msg->par("src");            // Old BTS
            iMS = msg->par("ms");                    // MS
            iWatts[iMS] = msg->par("watt");            // Current watt
            if (iWatts[iMS] < 0)     // if current is below zero (no connection)
                    {
                iBTS[iMS] = -1;             // Set the BTS identifier to invalid
            } else {
                iBTS[iMS] = iClientAddr;            // Set the old BTS number
            }
            for (i = 0; i < num_bts; i++)      // Send check request to all BTSs
                    {
                if (i != iClientAddr) {
                    force_check_ms = new cMessage("FORCE_CHECK_MS",
                            FORCE_CHECK_MS);
                    force_check_ms->addPar(*new cPar("dest")) = i;
                    force_check_ms->addPar(*new cPar("src")) = 0;
                    force_check_ms->addPar(*new cPar("ms")) = iMS;
                    send(force_check_ms, "to_bts", i);
                    ev << "Sending FORCE_CHECK_MS to BTS #" << i << "\n";
                };
            }
            // Send a scheduled message to myself to decide the handover
            handover_end = new cMessage("HANDOVER_END", HANDOVER_END);
            handover_end->addPar(*new cPar("ms")) = iMS;
            handover_end->addPar(*new cPar("bts")) = iClientAddr;
            counter = simTime() + delay;
            scheduleAt(counter, handover_end);
            break;

        case HANDOVER_DATA:                            // Watt data from BTS
            iClientAddr = msg->par("src");
            iMS = msg->par("ms");
            iWatt = msg->par("watt");
            ev << "got HANDOVER_DATA: Client: " << iClientAddr << " MS: " << iMS
                    << " Watt: " << iWatt << " oldWatt:" << iWatts[iMS] << "\n";
            if ((iWatt > iWatts[iMS]) && (iWatt > 0)) // if it's better then the old then save it
                    {
                iWatts[iMS] = iWatt;
                iBTS[iMS] = iClientAddr;
            }
            break;

        }
    }
}

