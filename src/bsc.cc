//-------------------------------------------------------------
// file: bsc.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"
#include "bsc.h"
#include "bts.h"
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
    isRouted = false;
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
    if(!isRouted){
        buildRoutingTable();
        isRouted = true;
    }
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
    const char* iMS = msg->par("ms");
    std::string iMSstr = std::string(iMS);// MS identifier
    const char* bts = msg->par("bts");                    // old BTS identifier
    cMessage *handover_bts_disc;
    if (strcmp(MStoBTS[iMSstr].c_str(),"no_bts")!= 0)           // if there is a BTS for the MS
            {
        if (strcmp(MStoBTS[iMSstr].c_str(),bts) !=0)    // not the original
                {
            // Send the handover message to the old BTS
            ev << " MS: " << iMS << " origBTS: " << bts << " newBTS: "
                    << MStoBTS[iMSstr].c_str() << "\n";
            handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
                    HANDOVER_BTS_DISC);
            handover_bts_disc->addPar(*new cMsgPar("newbts")) = MStoBTS[iMSstr].c_str();
            handover_bts_disc->addPar(*new cMsgPar("src")) = "BSC";
            handover_bts_disc->addPar(*new cMsgPar("dest")) = bts;
            handover_bts_disc->addPar(*new cMsgPar("ms")) = iMS;
            sendToBTS(handover_bts_disc, bts);
        }
    } else {
        // No BTS to hand over, send disconnect to BTS and MS
        handover_bts_disc = new cMessage("HANDOVER_BTS_DISC",
                HANDOVER_BTS_DISC);
        handover_bts_disc->addPar(*new cMsgPar("newbts")) = "no_bts";
        handover_bts_disc->addPar(*new cMsgPar("src")) = "BSC";
        handover_bts_disc->addPar(*new cMsgPar("dest")) = bts;
        handover_bts_disc->addPar(*new cMsgPar("ms")) = iMS;
        sendToBTS(handover_bts_disc, bts);
        //send(handover_bts_disc, "to_bts", i);
    }
}

void BSC::processHandoverCheck(cMessage *msg)
{
    bubble("HandoverCheck");
    const char * iClientAddr = msg->par("src");            // Old BTS
    const char * iMS = msg->par("ms");
    std::string iMSstr = std::string(iMS);// MS
    MSRSSI[iMSstr] = msg->par("watt").doubleValue();            // Current watt
    cMessage *force_check_ms;
    simtime_t counter = simTime();

    if (MSRSSI[iMSstr] == -1)     // if current is -1 (no connection)
    {
        MStoBTS[iMSstr] = "no_bts";             // Set the BTS identifier to invalid
    } else {
        MStoBTS[iMSstr] = std::string(iClientAddr);            // Set the old BTS number
    }

    int n = gateSize("to_bts");
    for (int i = 0; i < n; i++){      // Send check request to all BTSs
        if (i != routingTable[std::string(iClientAddr)]) {
            force_check_ms = new cMessage("FORCE_CHECK_MS",
                    FORCE_CHECK_MS);
            force_check_ms->addPar(*new cMsgPar("dest")) = "bcast";
            force_check_ms->addPar(*new cMsgPar("src")) = "BSC";
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
    const char* iClientAddr = msg->par("src");
    const char* iMS = msg->par("ms");
    std::string iMSstr = std::string(iMS);
    double rssi = msg->par("watt");
    EV << "got HANDOVER_DATA: Client: " << iClientAddr << " MS: " << iMS
            << " RSSI: " << rssi << " oldRSSI:" << MSRSSI[iMSstr] << "\n";
    if ((rssi > MSRSSI[iMSstr]) && (rssi != -1)) // if it's better then the old then save it
            {
        MSRSSI[iMSstr] = rssi;
        MStoBTS[iMSstr] = std::string(iClientAddr);
    }
}

void BSC::sendToBTS(cMessage *msg, const char* BCC)
{
    int portNum = routingTable[std::string(BCC)];
    send(msg, "to_bts", portNum);
}


// Maps the ports to the BTS addresses, so it only has to be done once.
void BSC::buildRoutingTable()
{
    int n = gateSize("to_bts");
    for(int x=0; x<n; x++){
        const char* address = "hello";
        cModule * refBTS;
        BTS * refBTS2;
        refBTS = gate("to_bts",x)->getNextGate()->getOwnerModule()->getSubmodule("bts_logic", -1);
        refBTS2 = dynamic_cast<BTS*> (refBTS);
        address = refBTS2->bcc;
        routingTable[std::string(address)] = x;
        EV << "BTS bcc: " << address << "is now mapped to port " << x << "\n";
    }

}

