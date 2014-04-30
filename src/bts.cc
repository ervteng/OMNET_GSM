//-------------------------------------------------------------
// file: bts.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"
#include "bts.h"
#include <math.h>
#include "GSMRadio.h"
#include "Radio80211aControlInfo_m.h"

Define_Module(BTS);

//simsignal_t BTS::connReqFromMsSignal = SIMSIGNAL_NULL;
//simsignal_t BTS::sendBeaconSignal    = SIMSIGNAL_NULL;
//simsignal_t BTS::checkLineFromMsSignal = SIMSIGNAL_NULL;

BTS::BTS() : cSimpleModule() {}
BTS::~BTS()
{
    cancelAndDelete(beaconTrigger);
}

void BTS::initialize()
{
    ev << "Calling Initialize()...\n";

    dblWatt = par("watt");                      // Watt
    dblRadius = dblWatt * WATT_MULTIPLY;        // Radius
    numSlots = par("slots");           // How many connection can hold the bts
    numPhones = par("phones");                // Number of phones
    iPhoneState = new int[numPhones];             // Allocate buffer
    numConnections = 0;                           // Number of connections
    bcc = par("BCC");
    beaconInterval = par("beaconInterval");
    beaconTrigger = new cMessage("SEND_BEACON");    // send the first scheduled move message

    double startTime = par("turnOnTime");
    scheduleAt(simTime()+startTime,beaconTrigger);
    EV << "BTS will turn on at "<<startTime << endl;

    // Register Signals
    connReqFromMsSignal = registerSignal("connReqFromMs");
    sendBeaconSignal = registerSignal("sendBeacon");
    numConnectionsSignal = registerSignal("numConnections");
    errNoSlotSignal = registerSignal("errNoSlot");
    handOverToBscSignal = registerSignal("handOverToBsc");
    handOverToMsSignal = registerSignal("handOverToMs");
    handOverCheckToBscSignal = registerSignal("handOverCheckToBsc");
    handOverCheckFromMsSignal = registerSignal("handOverCheckFromMs");
    forceDiscSignal = registerSignal("forceDisc");
    discRequestSignal = registerSignal("discRequest");

    // Error
    errNoSlot = 0;
    connReqFromMsCount = 0;
    handOverToBscCount = 0;
    handOverToMsCount = 0;
    handOverCheckFromMsCount = 0;
    handOverCheckToBscCount = 0;

}

// Get called to record statistics
void BTS::finish()
{
    ev << "Calling finished()...\n";
    simtime_t t = simTime();
    recordScalar("simulated time", t);
//    delete handover_ms, force_disc;
}

void BTS::destroy() {
    if (iPhoneState)
        delete iPhoneState;            // Release dynamic buffer
    //if(connectedPhones)
    //delete connectedPhones;
}

double BTS::getRSSIFromPacket(cMessage *msg) {

    Radio80211aControlInfo *cinfo = dynamic_cast<Radio80211aControlInfo*>(msg->getControlInfo());
    double recPower  = 0;
    if(cinfo){
        recPower = cinfo->getRecPow();
    }

    if (recPower > -110)                    // if it sees the ms
    {
         //return ((dblRadius - dblDistance) * dblWatt / dblRadius);
        return recPower;
    }
    else {
        return -1;
    }
}


void BTS::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){
        EV << "Time to send a Beacon!\n";
        sendBeacon();
    }
    else if(!isOn){
            delete msg;
            return;
    }
    else{
        const char* intendedDest = msg->par("dest");
        if(strcmp(intendedDest,bcc) != 0 && strcmp(intendedDest,"bcast") !=0){
            EV << "This message isn't for me! Dropping\n";
            delete msg;  // keeps OMNeT++ happy
            return;
        }
        if (msg->getKind() == CONN_REQ){ // Connection request from MS
            processMsgConnReqFromMs(msg);
        }else if (msg->getKind() == CHECK_BTS){  // Check request from the MS
            processMsgCheckBtsFromMs(msg);
        }else if (msg->getKind() == DISC_REQ){ // Disconnect request from MS
            processMsgDiscReqFromMs(msg);
        }else if (msg->getKind() == HANDOVER_BTS_DISC){ // Handover request from BSC
            processMsgHandoverFromBsc(msg);
        }else if (msg->getKind() == FORCE_CHECK_MS){ // Check MS for handover
            processMsgForceCheckMsFromBsc(msg);
        }else if (msg->getKind() == MS_DATA){ // Requested data from MS for handover
            processMsgDataFromMs(msg);
        }else if (msg->getKind() == CHECK_LINE){ // Request from MS to check handover
            processMsgCheckLineFromMs(msg);
        }else {
            ev << "==> [BTS] Message Error: Cannot handle message\n";
        }
    }
}

void BTS::processMsgConnReqFromMs(cMessage *msg)
{
    EV << "==> [BTS] RCV: CONN_REQ from MS\n";
    const char* iClientAddr = msg->par("src");
    if (numConnections < numSlots)                // If there is a free slot
    {
        EV << "==> [BTS] client is addr=" << iClientAddr
           << ", sending CONN_ACK free slots left:"
           << numSlots - numConnections - 1 << '\n';
        msg->setName("CONN_ACK");
        msg->setKind(CONN_ACK);
        msg->par("dest") = iClientAddr;
        send(msg, "to_air");
        //iPhoneState[iClientAddr] = PHONE_STATE_CONNECTED; // Set the phone state to connected
        connectedPhones.insert(std::string(iClientAddr));
        numConnections++;    // Increase the number of current connections
        emit(numConnectionsSignal, numConnections);
    }else {
        EV << "==> [BTS] Error: no slot available\n";
        errNoSlot++;

        emit(errNoSlotSignal, errNoSlot);
    }

    connReqFromMsCount++;
    emit(connReqFromMsSignal, connReqFromMsCount);

}

void BTS::processMsgCheckBtsFromMs(cMessage *msg)
{
    EV << "==> [BTS] RCV: CHECK_BTS from MS\n";
    const char* iClientAddr = msg->par("src");
    const char* iDest = msg->par("dest");
    msg->setName("BTS_DATA");
    msg->setKind(BTS_DATA);
    msg->par("dest") = iClientAddr;
    msg->par("src") = bcc;
    double dblPower = getRSSIFromPacket(msg);
    EV << "==> [BTS] RSSI=" << dblPower << endl;
    if ((dblPower != -1) && (numConnections < numSlots)) // if it sees the ms and has free slot
    {
        msg->addPar(*new cMsgPar("data") = dblPower);
        EV << "==> [BTS] Sending data to " << iDest << endl;
        send(msg, "to_air");
    }else {
        errNoSlot++;
        emit(errNoSlotSignal, errNoSlot);
    }

//    // Stats
//    char signalName[32];
//    sprintf(signalName, "phone%s-rssi", iClientAddr);
//    simsignal_t signal = registerSignal(signalName);
//
//    char statisticName[32];
//    sprintf(statisticName, "phone%s-rssi", iClientAddr);
//    cProperty *statisticTemplate = getProperties()->get("statisticTemplate", "phoneRssi");
//    ev.addResultRecorders(this, signal, statisticName, statisticTemplate);
//    emit(signal, dblPower);

}

void BTS::processMsgDiscReqFromMs(cMessage *msg)
{
    ev << "==> [BTS] got DISC_REQ, sending DISC_ACK\n";
    const char* iClientAddr = msg->par("src");
    std::string iClientAddrStr = std::string(iClientAddr);
    msg->setName("DISC_ACK");
    msg->setKind(DISC_ACK);
    msg->par("dest") = iClientAddr;
    msg->par("src") = bcc;
    send(msg, "to_air");
    //iPhoneState[iClientAddr] = PHONE_STATE_DISCONNECTED; // Set the phone state to not connected
    if(connectedPhones.count(iClientAddrStr) > 0){
        connectedPhones.erase(iClientAddrStr);
    }
    EV << "Phone IMSI:"<<iClientAddr <<" is now disconnected \n";
    numConnections--;        // Decrease the number of current connections

    emit(numConnectionsSignal, numConnections);
    emit(discRequestSignal, 1);
}

void BTS::processMsgHandoverFromBsc(cMessage *msg)
{
    const char* iClientAddr = msg->par("src");
    std::string iClientAddrStr = std::string(iClientAddr);

    const char* iDest = msg->par("dest");
    const char* iNewBTS = msg->par("newbts");
    const char* iMS = msg->par("ms");
    EV << "Received Handover request from BSC. Process? "<< connectedPhones.count(std::string("hellothere"));
    if (connectedPhones.count(std::string(iMS)) != 0) {
        if (strcmp(iNewBTS,"no_bts")!=0)   // Disconnect MS and send the new bts number
                {
            cPacket *handover_ms = new cPacket("HANDOVER_MS", HANDOVER_MS);
            handover_ms->addPar(*new cMsgPar("newbts") = iNewBTS);
            handover_ms->addPar(*new cMsgPar("dest")) = iMS;
            handover_ms->addPar(*new cMsgPar("src")) = bcc;  //send to ms
            numConnections--;
            ev << "==> [BTS] Sending HANDOVER_MS to client " << iClientAddr
                    << ", free slots left:" << numSlots - numConnections
                    << '\n';
            send(handover_ms, "to_air");

            handOverToMsCount++;
            emit(handOverToMsSignal, handOverToMsCount);
        } else {                            // Disconnect MS immediately
            cPacket *force_disc = new cPacket("FORCE_DISC", FORCE_DISC);
            force_disc->addPar(new cMsgPar("dest")) = iMS;
            force_disc->addPar(new cMsgPar("src")) = bcc;   //send to ms
            numConnections--;
            ev << "==> [BTS] Sending FORCE_DISC to client " << iClientAddr
               << ", free slots left:" << numSlots - numConnections
               << '\n';
            send(force_disc, "to_air");

            emit(forceDiscSignal, 1);
        }
    }
    // Memory
    delete msg;
    emit(numConnectionsSignal, numConnections);
}

void BTS::processMsgForceCheckMsFromBsc(cMessage *msg)
{
    if (numConnections < numSlots)                // If there are free slots
    {
        const char* iMS = msg->par("ms");
       // const char* iDest = msg->par("dest");
        cPacket *check_ms = new cPacket("CHECK_MS", CHECK_MS);
        check_ms->addPar(*new cMsgPar("dest")) = iMS;
        check_ms->addPar(*new cMsgPar("src")) = bcc;
        ev << "Sending CHECK_MS to client " << iMS << '\n';
        send(check_ms, "to_air");
    }
    else {
        emit(errNoSlotSignal, errNoSlot);
    }
    delete msg;
}
void BTS::processMsgDataFromMs(cMessage *msg)
{
    const char* iClientAddr = msg->par("src");
    const char* iDest = msg->par("dest");
//    msg->setName("BTS_DATA");
//    msg->setKind(BTS_DATA);
//    msg->par("dest") = iClientAddr;
//    msg->par("src") = iDest;
    double dblPower = getRSSIFromPacket(msg);
    if ((dblPower != -1) && (numConnections < numSlots)) // if it sees the ms and has free slot
            {
        handover_data = new cPacket("HANDOVER_DATA", HANDOVER_DATA);
        handover_data->addPar(*new cMsgPar("ms") = iClientAddr);
        handover_data->addPar(*new cMsgPar("src") = bcc);
        handover_data->addPar(*new cMsgPar("watt") = dblPower);
        ev << "==> [BTS] Sending HANDOVER_DATA with watt " << dblPower
           << " to BSC\n";
        send(handover_data, "to_bsc");

        handOverToBscCount++;
        emit(handOverToBscSignal, handOverToBscCount);
    }
    else {
        emit(errNoSlotSignal, errNoSlot);
    }
    delete msg;
}

void BTS::processMsgCheckLineFromMs(cMessage *msg)
{
    const char* iClientAddr = msg->par("src");
    //const char* iDest = msg->par("dest");
    double dblPower = getRSSIFromPacket(msg);
    EV << "==> [BTS] RCV RSSI=" << dblPower << endl;

    emit(handOverCheckFromMsSignal, 1);
    if ((dblPower < 0) || (dblPower < dblWatt * HANDOVER_LIMIT)) {
        // Check for handover
        handover_chk = new cMessage("HANDOVER_CHK", HANDOVER_CHK);
        handover_chk->addPar(*new cMsgPar("ms") = iClientAddr);
        handover_chk->addPar(*new cMsgPar("src") = bcc);
        handover_chk->addPar(*new cMsgPar("watt") = dblPower);
        EV << "==> [BTS] Sending HANDOVER_CHK to BSC\n";
        send(handover_chk, "to_bsc");
        handOverCheckToBscCount++;
        emit(handOverCheckToBscSignal, handOverCheckToBscCount);
    }
    else {
        emit(errNoSlotSignal, errNoSlot);
    }
    delete msg;
}

void BTS::sendBeacon()
{
    bts_beacon = new cPacket("BTS_BEACON", BTS_BEACON);
    bts_beacon->addPar(*new cMsgPar("src") = bcc);
    bts_beacon->addPar(*new cMsgPar("dest") = "all");
    send(bts_beacon, "to_air");

    emit(sendBeaconSignal, 1);
    isOn = true;
       // send the first scheduled move message
    // The Uniform is there to prevent the beacons from colliding. The real way is to use channel
    // selection, but we do this in the interest of time.
    scheduleAt(simTime()+ beaconInterval + uniform(0,0.1),beaconTrigger);
}
