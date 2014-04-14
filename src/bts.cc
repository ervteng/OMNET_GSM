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

BTS::BTS() : cSimpleModule() {

}

void BTS::initialize()
{
    ev << "Calling Initialize()...\n";
    dblXc = par("xc");                          // X coordinate
    dblYc = par("yc");                          // Y coordinate
    dblWatt = par("watt");                      // Watt
    dblRadius = dblWatt * WATT_MULTIPLY;        // Radius
    iSlots = par("slots");           // How many connection can hold the bts
    iPhones = par("phones");                // Number of phones
    iPhoneState = new int[iPhones];             // Allocate buffer
    iConnections = 0;                           // Number of connections
}

// Get called to record statistics
void BTS::finish()
{
    ev << "Calling finished()...\n";
}

void BTS::destroy() {
    if (iPhoneState)
        delete iPhoneState;            // Release dynamic buffer
}

double BTS::CalculateWatt(double dblMSXc, double dblMSYc, double recPower) {
    double dblDistance;

    dblDistance = sqrt(
            (dblXc - dblMSXc) * (dblXc - dblMSXc)
                    + (dblYc - dblMSYc) * (dblYc - dblMSYc));

    //GSMRadio* myRadio =  (GSMRadio*) this->getParentModule()->getSubmodule("radio");

    //EV << "ParentModule is" << myRadio;
    //GSMRadio* myRadio2 = dynamic_cast<GSMRadio *>(myRadio);
    //double rssi = 0;
    //rssi = myRadio->getRSSI();
//    if (dblDistance < dblRadius)                    // if it sees the ms
//            {
//        return ((dblRadius - dblDistance) * dblWatt / dblRadius);
//    }
    //EV << "Current received signal is" << rssi << "\n";
    if (recPower > 10)                    // if it sees the ms
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

    }else if (msg->getKind() == CONN_REQ){ // Connection request from MS
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
        ev << "Message Error: Cannot handle message\n";
    }
}

void BTS::processMsgConnReqFromMs(cMessage *msg)
{
    int iClientAddr = msg->par("src");
    if (iConnections < iSlots)                // If there is a free slot
    {
        ev << "client is addr=" << iClientAddr
           << ", sending CONN_ACK free slots left:"
           << iSlots - iConnections - 1 << '\n';
        msg->setName("CONN_ACK");
        msg->setKind(CONN_ACK);
        msg->par("dest") = iClientAddr;
        send(msg, "to_air");
        iPhoneState[iClientAddr] = PHONE_STATE_CONNECTED; // Set the phone state to connected
        iConnections++;    // Increase the number of current connections
    }
}

void BTS::processMsgCheckBtsFromMs(cMessage *msg)
{
    int iClientAddr = msg->par("src");
    int iDest = msg->par("dest");
    double dblMSXc = msg->par("xc");
    double dblMSYc = msg->par("yc");
    msg->setName("BTS_DATA");
    msg->setKind(BTS_DATA);
    msg->par("dest") = iClientAddr;
    msg->par("src") = iDest;
    double dblPower = CalculateWatt(dblMSXc, dblMSYc);
    if ((dblPower > 0) && (iConnections < iSlots)) // if it sees the ms and has free slot
            {
        msg->addPar(*new cMsgPar("data") = dblPower);
        ev << "Sending data to " << iDest;
        send(msg, "to_air");
    }
}

void BTS::processMsgDiscReqFromMs(cMessage *msg)
{
    ev << "got DISC_REQ, sending DISC_ACK\n";
    int iClientAddr = msg->par("src");
    msg->setName("DISC_ACK");
    msg->setKind(DISC_ACK);
    msg->par("dest") = iClientAddr;
    send(msg, "to_air");
    iPhoneState[iClientAddr] = PHONE_STATE_DISCONNECTED; // Set the phone state to not connected
    iConnections--;        // Decrease the number of current connections
}

void BTS::processMsgHandoverFromBsc(cMessage *msg)
{
    int iClientAddr = msg->par("src");
    int iDest = msg->par("dest");
    int iNewBTS = msg->par("newbts");
    int iMS = msg->par("ms");
    if (iPhoneState[iMS] == PHONE_STATE_CONNECTED) {
        if (iNewBTS > -1)   // Disconnect MS and send the new bts number
                {
            cMessage *handover_ms = new cMessage("HANDOVER_MS", HANDOVER_MS);
            handover_ms->addPar(*new cMsgPar("newbts") = iNewBTS);
            handover_ms->addPar(*new cMsgPar("dest")) = iMS;
            handover_ms->addPar(*new cMsgPar("src")) = iDest;  //send to ms
            iConnections--;
            ev << "Sending HANDOVER_MS to client " << iClientAddr
                    << ", free slots left:" << iSlots - iConnections
                    << '\n';
            send(handover_ms, "to_air");
        } else {                            // Disconnect MS immediately
            cMessage *force_disc = new cMessage("FORCE_DISC", FORCE_DISC);
            force_disc->addPar(new cMsgPar("dest")) = iMS;
            force_disc->addPar(new cMsgPar("src")) = iDest;   //send to ms
            iConnections--;
            ev << "Sending FORCE_DISC to client " << iClientAddr
               << ", free slots left:" << iSlots - iConnections
               << '\n';
            send(force_disc, "to_air");
        }
    }
}

void BTS::processMsgForceCheckMsFromBsc(cMessage *msg)
{
    if (iConnections < iSlots)                // If there are free slots
    {
        int iMS = msg->par("ms");
        int iDest = msg->par("dest");
        cMessage *check_ms = new cMessage("CHECK_MS", CHECK_MS);
        check_ms->addPar(*new cMsgPar("dest")) = iMS;
        check_ms->addPar(*new cMsgPar("src")) = iDest;
        ev << "Sending CHECK_MS to client " << iMS << '\n';
        send(check_ms, "to_air");
    }
}
void BTS::processMsgDataFromMs(cMessage *msg)
{
    int iClientAddr = msg->par("src");
    int iDest = msg->par("dest");
    double dblMSXc = msg->par("xc");
    double dblMSYc = msg->par("yc");
    msg->setName("BTS_DATA");
    msg->setKind(BTS_DATA);
    msg->par("dest") = iClientAddr;
    msg->par("src") = iDest;
    double dblPower = CalculateWatt(dblMSXc, dblMSYc);
    if ((dblPower > 0) && (iConnections < iSlots)) // if it sees the ms and has free slot
            {
        cMessage *handover_data = new cMessage("HANDOVER_DATA", HANDOVER_DATA);
        handover_data->addPar(*new cMsgPar("ms") = iClientAddr);
        handover_data->addPar(*new cMsgPar("src") = iDest);
        handover_data->addPar(*new cMsgPar("watt") = dblPower);
        ev << "Sending HANDOVER_DATA with watt " << dblPower
           << " to BSC\n";
        send(handover_data, "to_bsc");
    }
}

void BTS::processMsgCheckLineFromMs(cMessage *msg)
{
    int iClientAddr = msg->par("src");
    int iDest = msg->par("dest");
    double dblMSXc = msg->par("xc");
    double dblMSYc = msg->par("yc");
    double dblPower = CalculateWatt(dblMSXc, dblMSYc);

    ev << " Watt:" << dblPower << " Watt limit "
       << dblWatt * HANDOVER_LIMIT << "\n";

    if ((dblPower < 0) || (dblPower < dblWatt * HANDOVER_LIMIT)) {
        // Check for handover
        cMessage *handover_chk = new cMessage("HANDOVER_CHK", HANDOVER_CHK);
        handover_chk->addPar(*new cMsgPar("ms") = iClientAddr);
        handover_chk->addPar(*new cMsgPar("src") = iDest);
        handover_chk->addPar(*new cMsgPar("watt") = dblPower);
        ev << "Sending HANDOVER_CHK to BSC\n";
        send(handover_chk, "to_bsc");
    }
}

//void BTS::activity() {
//    cMessage *msg, *handover_chk, *handover_ms, *force_disc, *handover_data;
//    cMessage *check_ms;
//    int iType, iClientAddr = 0, iDest = 0;
//
//    double dblMSXc, dblMSYc, dblPower;
//    int iNewBTS;
//    int iMS;
//
//    for (;;) {
//        // receive message
//        msg = receive();
//        iType = msg->getKind();
//        iClientAddr = msg->par("src");
//        iDest = msg->par("dest");
//        switch (iType) {
//        case CONN_REQ:                             // Connection request from MS
//            if (iConnections < iSlots)                // If there is a free slot
//                    {
//                ev << "client is addr=" << iClientAddr
//                        << ", sending CONN_ACK free slots left:"
//                        << iSlots - iConnections - 1 << '\n';
//                msg->setName("CONN_ACK");
//                msg->setKind(CONN_ACK);
//                msg->par("dest") = iClientAddr;
//                send(msg, "to_air");
//                iPhoneState[iClientAddr] = PHONE_STATE_CONNECTED; // Set the phone state to connected
//                iConnections++;    // Increase the number of current connections
//            }
//            break;
//
//        case CHECK_BTS:                             // Check request from the MS
//            dblMSXc = msg->par("xc");
//            dblMSYc = msg->par("yc");
//            msg->setName("BTS_DATA");
//            msg->setKind(BTS_DATA);
//            msg->par("dest") = iClientAddr;
//            msg->par("src") = iDest;
//            dblPower = CalculateWatt(dblMSXc, dblMSYc);
//            if ((dblPower > 0) && (iConnections < iSlots)) // if it sees the ms and has free slot
//                    {
//                msg->addPar(*new cMsgPar("data") = dblPower);
//                ev << "Sending data to " << iDest;
//                send(msg, "to_air");
//            }
//            break;
//
//        case DISC_REQ:                             // Disconnect request from MS
//            ev << "got DISC_REQ, sending DISC_ACK\n";
//            msg->setName("DISC_ACK");
//            msg->setKind(DISC_ACK);
//            msg->par("dest") = iClientAddr;
//            send(msg, "to_air");
//            iPhoneState[iClientAddr] = PHONE_STATE_DISCONNECTED; // Set the phone state to not connected
//            iConnections--;        // Decrease the number of current connections
//            break;
//
//        case HANDOVER_BTS_DISC:                     // Handover request from BSC
//            iNewBTS = msg->par("newbts");
//            iMS = msg->par("ms");
//            if (iPhoneState[iMS] == PHONE_STATE_CONNECTED) {
//                if (iNewBTS > -1)   // Disconnect MS and send the new bts number
//                        {
//                    handover_ms = new cMessage("HANDOVER_MS", HANDOVER_MS);
//                    handover_ms->addPar(*new cMsgPar("newbts") = iNewBTS);
//                    handover_ms->addPar(*new cMsgPar("dest")) = iMS;
//                    handover_ms->addPar(*new cMsgPar("src")) = iDest;  //send to ms
//                    iConnections--;
//                    ev << "Sending HANDOVER_MS to client " << iClientAddr
//                            << ", free slots left:" << iSlots - iConnections
//                            << '\n';
//                    send(handover_ms, "to_air");
//                } else {                            // Disconnect MS immediately
//                    force_disc = new cMessage("FORCE_DISC", FORCE_DISC);
//                    force_disc->addPar(new cMsgPar("dest")) = iMS;
//                    force_disc->addPar(new cMsgPar("src")) = iDest;   //send to ms
//                    iConnections--;
//                    ev << "Sending FORCE_DISC to client " << iClientAddr
//                            << ", free slots left:" << iSlots - iConnections
//                            << '\n';
//                    send(force_disc, "to_air");
//                }
//            }
//            break;
//
//        case FORCE_CHECK_MS:                        // Check MS for handover
//            if (iConnections < iSlots)                // If there are free slots
//                    {
//                iMS = msg->par("ms");
//                check_ms = new cMessage("CHECK_MS", CHECK_MS);
//                check_ms->addPar(*new cMsgPar("dest")) = iMS;
//                check_ms->addPar(*new cMsgPar("src")) = iDest;
//                ev << "Sending CHECK_MS to client " << iMS << '\n';
//                send(check_ms, "to_air");
//            }
//            break;
//
//        case MS_DATA:                     // Requested data from MS for handover
//            dblMSXc = msg->par("xc");
//            dblMSYc = msg->par("yc");
//            msg->setName("BTS_DATA");
//            msg->setKind(BTS_DATA);
//            msg->par("dest") = iClientAddr;
//            msg->par("src") = iDest;
//            dblPower = CalculateWatt(dblMSXc, dblMSYc);
//            if ((dblPower > 0) && (iConnections < iSlots)) // if it sees the ms and has free slot
//                    {
//                handover_data = new cMessage("HANDOVER_DATA", HANDOVER_DATA);
//                handover_data->addPar(*new cMsgPar("ms") = iClientAddr);
//                handover_data->addPar(*new cMsgPar("src") = iDest);
//                handover_data->addPar(*new cMsgPar("watt") = dblPower);
//                ev << "Sending HANDOVER_DATA with watt " << dblPower
//                        << " to BSC\n";
//                send(handover_data, "to_bsc");
//            }
//            break;
//
//        case CHECK_LINE:                    // Request from MS to check handover
//            dblMSXc = msg->par("xc");
//            dblMSYc = msg->par("yc");
//            dblPower = CalculateWatt(dblMSXc, dblMSYc);
//            ev << " Watt:" << dblPower << " Watt limit "
//                    << dblWatt * HANDOVER_LIMIT << "\n";
//            if ((dblPower < 0) || (dblPower < dblWatt * HANDOVER_LIMIT)) {
//                // Check for handover
//                handover_chk = new cMessage("HANDOVER_CHK", HANDOVER_CHK);
//                handover_chk->addPar(*new cMsgPar("ms") = iClientAddr);
//                handover_chk->addPar(*new cMsgPar("src") = iDest);
//                handover_chk->addPar(*new cMsgPar("watt") = dblPower);
//                ev << "Sending HANDOVER_CHK to BSC\n";
//                send(handover_chk, "to_bsc");
//            }
//            break;
//        }
//    }
//}
//
