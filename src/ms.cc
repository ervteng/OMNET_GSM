//-------------------------------------------------------------
// file: ms.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"
#include "ms.h"
#include "GSMRadio.h"
#include "Radio80211aControlInfo_m.h"

Define_Module(MS);

MS::MS(): cSimpleModule() {}

MS::~MS()
{
    //delete conn_req,disc_req,allmsg;
    //delete check_bts,check_ms,check_line;
    cancelAndDelete(nextCall);
    cancelAndDelete(movecar);
    cancelAndDelete(scanChannelsStart);
    cancelAndDelete(scanChannelsStop);
}

void MS::initialize(){
    status=TRIGGER;                            // program state                          // number of handovers
    delay=1.0;

    // Stats counter
    missedCalls = 0;
    attemptedCalls = 0;
    brokenCalls = 0;
    handOverCalls = 0;
    successfullCalls = 0;

    connected = "no_bts";
    callinterval = CALL_INTERVAL_MIN + exponential(CALL_INTERVAL);
    calllength = CALL_LENGTH_MIN + exponential(CALL_LENGTH);
    timeout = par ( "timeout" );
    imsi = par("imsi");
    selected="no_bts";
    num_bts = 0;
    beaconListenInterval = par("beaconListenInterval");
    movecar = new cPacket("TRIGGER",TRIGGER);    // send the first scheduled move message
    //SimTime counter = simTime()+delay;

    nextCall = new cPacket("make_call");
    scheduleAt(callinterval+0.3, nextCall);
    //scanChannels = new cMessage( "START_SCANNING",START_SCANNING);
    //scheduleAt(simTime()+0.1,scanChannels);
    counter = simTime()+delay;
    lastBeaconUpdate = simTime();
    scheduleAt(simTime(),movecar);
    min = 0;

    // Set up selfmessages
    scanChannelsStart = new cMessage( "START_SCANNING",START_SCANNING);
    scanChannelsStop = new cMessage( "STOP_SCANNING",STOP_SCANNING);

    scheduleAt(simTime()+0.1,scanChannelsStart);
    // Set up instrumentation
    beaconRSSIsignal = registerSignal("receivedRSSI");
    attemptedCallSignal = registerSignal("attemptedCall");
    successfulCallSignal = registerSignal("successfulCall");
    missedCallSignal = registerSignal("missedCall");
    brokenCallSignal = registerSignal("brokenCall");
    handoverCallSignal = registerSignal("handoverCall");
    // FOR DEBUGGING ONLY
    //callinterval = 0.3;

}


void MS::finish()
{
//    Ev << "[MS] Calling finish()\n";
}

void MS::receiveChangeNotification(int category, const cObject *details)
{
    EV << "------------ receiveChangeNotification ----------\n";
    EV << category << endl;
}

void MS::handleMessage(cMessage *msg)
{
    type = msg->getKind();
    EV << "Message is " << type << "\n";
    if(msg->isSelfMessage()){
        if (type == TRIGGER){ // TODO Replace with selfmessage logic
            processMsgTrigger(msg);
        }
        else if(type == START_SCANNING){
            startScanning();
        }
        else if(type == STOP_SCANNING){
            stopScanning();
        }
        //delete msg;
    }
    else if(type == BTS_BEACON){
        processMsgBtsBeacon(msg);
    }
    else {
        const char* intendedDest = msg->par("dest");
        if(strcmp(intendedDest,imsi) != 0){
            EV << "This message isn't for me! Dropping";
            delete msg;  // keeps OMNeT++ happy
            return;
        }
        if (type == BTS_DATA){
            processMsgBtsData(msg);
        }else if (type == CONN_ACK){
            processMsgConnAck(msg);
        }else if (type == DISC_ACK){
            processMsgDiscAck(msg);
        }else if (type == FORCE_DISC){
            processMsgForceDisc(msg);
        }else if (type == CHECK_MS){
            processMsgCheckMs(msg);
        }else if (type == HANDOVER_MS){
            processMsgHandoverMs(msg);
        }else {
            EV <<  "==> MS[" << imsi << "] ERROR: no matched message type=" << type;
        }
    }


    if ((alltime>=(double)callinterval)&&(status==TRIGGER)) {

        //Send connetion request to currently selected station
        //min=0.0;selected="no_bts";

        EV << "Time to attempt a call! sending message to " << selected <<endl;
        if(selected=="no_bts")
        {
            EV << "We have no reception here. Sucks."<<endl;
        }
        else{
            alltime=0;
            check_bts = new cPacket( "CHECK_BTS", CHECK_BTS );
            check_bts->addPar( *new cMsgPar("src")  = imsi );
            check_bts->addPar( *new cMsgPar("dest") = selected.c_str() );
            ev << "Sending CHECK_BTS to all BTS" << '\n';
            send( check_bts, "to_air" );            // Send the first BTS check message
            status=CHECK_BTS;                        // New state
            lastmsg=simTime();
        }
    };

    // If it's due then terminate the call
    if ((alltime>=(double)calllength)&&(status==CONN_ACK)) {
        ev << "==> MS[" << imsi << "] sending DISC_REQ\n";
        disc_req = new cPacket( "DISC_REQ", DISC_REQ );
        disc_req->addPar("src") = imsi;
        disc_req->addPar("dest") = connected.c_str();
        send( disc_req, "to_air" );                // Send disconnect request to the BTS
        lastmsg=simTime();
        status=DISC_REQ;
    };
}


void MS::processMsgTrigger(cMessage *msg)
{

    alltime+=delay;
    //movecar = new cPacket("TRIGGER",TRIGGER);// send new scheduled message
    counter = simTime()+delay;
    scheduleAt(counter,movecar);
    switch (status){
    case CHECK_BTS:                            // check if timeout happened
        if (lastmsg+timeout>=simTime())
        {
//            if (i<num_bts-1)                // if not all BTS checked
//            {
//                i++;                        // send new check message
//                check_bts=new cPacket("CHECK_BTS",CHECK_BTS);
//                check_bts->addPar( *new cMsgPar("src") =imsi);
//                check_bts->addPar( *new cMsgPar("dest") ="bcast");
//                ev << "==> MS[" << imsi << "] Sending CHECK_BTS to #" << i << '\n';
//                send(check_bts,"to_air");
//                lastmsg=simTime();
//            } else
            if (selected != "no_bts")                // there's an available BTS
            {
                ev << "==> MS[" << imsi << "] sending CONN_REQ to BTS: " << selected << '\n';
                conn_req = new cPacket( "CONN_REQ",CONN_REQ);
                conn_req->addPar( *new cMsgPar("src") =imsi);
                conn_req->addPar( *new cMsgPar("dest")=selected.c_str());
                send( conn_req, "to_air" );    // Send connection request
                status=CONN_REQ;
                lastmsg=simTime();
                attemptedCalls++;
                emit(attemptedCallSignal, attemptedCalls);

            } else
            {                                // no available BTS
                attemptedCalls++;
                emit(attemptedCallSignal, attemptedCalls);

                missedCalls++;
                emit(missedCallSignal, missedCalls);

                ev << "==> MS[" << imsi << "] Can't connect (CHECK_BTS). Error #" << missedCalls;
                // calculate new parameters
                callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
                calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
                status=TRIGGER;

            };
        };
        break;

    case CONN_REQ:                            // Waiting for connection ack
        if (lastmsg+timeout>=simTime())
        {
            if (connected != "no_bts")                // If we were connected before
            {
                // Broken handover
                brokenCalls++;
                emit(brokenCallSignal, brokenCalls);
            }
            else
            {
                // Can't make a new call
                missedCalls++;
                emit(missedCallSignal, missedCalls);

            }
            ev << "==> MS[" << imsi << "] Can't connect (CONN_REQ). Error #" << missedCalls;

            // Calculate new call parameters
            callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
            calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
            status=TRIGGER;
        };
        break;

    case CONN_ACK:                            // Check for handover periodically
        successfullCalls++;
        emit(successfulCallSignal, successfullCalls);
        check_line = new cPacket("CHECK_LINE",CHECK_LINE);
        check_line->addPar( *new cMsgPar("src") = imsi);
        check_line->addPar( *new cMsgPar("dest") = connected.c_str() );
        ev << "==> MS[" << imsi << "] Sending CHECK_LINE to #" << connected << '\n';
        send(check_line,"to_air");            // Sending CHECK_LINE message to BTS
        break;
    };

}

void MS::processMsgBtsData(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got BTS_DATA\n";
    double rssi = getRSSIFromPacket(msg);
    EV << "==> MS[" << imsi << "] RCV Rssi=" << rssi << endl;
    const char * testing = msg->par("src");
    EV << "FROM BTS " <<selected <<endl;
    emit(beaconRSSIsignal, rssi);
//    if (i < num_bts-1){                        // If not all BTS checked
//        i++;
//        check_bts = new cPacket("CHECK_BTS",CHECK_BTS);
//        check_bts->addPar( *new cMsgPar("src") = imsi);
//        check_bts->addPar( *new cMsgPar("dest") = i );
//        ev << "==> MS[" << imsi << "] Sending CHECK_BTS to #" << i << '\n';
//        send(check_bts,"to_air");            // Send check_bts to the next BTS
//        lastmsg=simTime();                    // Set the time of last msg
    if (selected != "no_bts") {                       // We have a good BTS, let's connect
        ev << "==> MS[" << imsi << "] sending CONN_REQ to #" << selected << '\n';
        conn_req = new cPacket( "CONN_REQ",CONN_REQ);
        conn_req->addPar( *new cMsgPar("src") = imsi);
        conn_req->addPar( *new cMsgPar("dest") = selected.c_str());
        send( conn_req, "to_air" );            // Send connection request
        status=CONN_REQ;
        lastmsg=simTime();
        attemptedCalls++;
        emit(attemptedCallSignal, attemptedCalls);
    } else {                                        // No answer from BTSs
        attemptedCalls++;
        emit(attemptedCallSignal, attemptedCalls);
        missedCalls++;
        emit(missedCallSignal, missedCalls);

        ev << "Can't connect. (CHECK_BTS) Error #" << missedCalls;

        // Calculate new call parameters
        callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
        calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
        status=TRIGGER;
    }
    delete msg;
}

void MS::processMsgConnAck(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got CONN_ACK\n";
    const char* currStation =msg->par("src");
    connected = selected;                    // Connected to this BTS
    ev << "==> MS[" << imsi << "] Talking with station #" << currStation << '\n';
    status=CONN_ACK;
    lastmsg=simTime();
    alltime=0;                                // Reset the timer
}

void MS::processMsgDiscAck(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got DISC_ACK\n";
    lastmsg=simTime();
    alltime=0;
    status=TRIGGER;

    // Calculate new call parameters
    callinterval = CALL_INTERVAL + exponential(CALL_INTERVAL_MIN);
    calllength = CALL_LENGTH_MIN + exponential(CALL_LENGTH);
    connected = "no_bts";
}

void MS::processMsgForceDisc(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got FORCE_DISC\n";
    lastmsg=simTime();
    alltime=0;
    status=TRIGGER;

    // Calculate new call parameters
    callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
    calllength = CALL_RETRY_LENGTH_MIN /2 + exponential(CALL_RETRY_LENGTH) /2;
    connected = "no_bts";                            // Not connected to a BTS
    brokenCalls++;
    emit(brokenCallSignal, brokenCalls);

}

void MS::processMsgCheckMs(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got CHECK_MS\n";
    check_ms = new cPacket( "MS_DATA",MS_DATA);
    check_ms->addPar( *new cMsgPar("src") = imsi);
    check_ms->addPar( *new cMsgPar("dest") = msg->par("src"));
    send( check_ms, "to_air" );                // Send back the current position
    delete msg;
}

void MS::processMsgHandoverMs(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got HANDOVER_MS \n";
    if (connected != "no_bts"){
        ev << "got HANDOVER_MS \n";
        selected = std::string(msg->par("newbts"));
        ev << "sending CONN_REQ to BCC " << selected << '\n';
        conn_req = new cPacket( "CONN_REQ",CONN_REQ);// Sending connection request to new bts
        conn_req->addPar( *new cMsgPar("src") = imsi);
        conn_req->addPar( *new cMsgPar("dest") = selected.c_str());
        send( conn_req, "to_air" );
        status=CONN_REQ;
        lastmsg=simTime();
        handOverCalls++;
        emit(handoverCallSignal, handOverCalls);

    }
    delete msg;
}

void MS::processMsgBtsBeacon(cMessage *msg)
{
    if(scanStatus==true && status == TRIGGER )
    {// if the last time we updated from a beacon was too long ago
        ev << "==> MS[" << imsi << "] got BEACON\n";
        double rssi = getRSSIFromPacket(msg);
        EV << "==> MS[" << imsi << "] RCV Rssi=" << rssi << endl;
        if(rssi > min){
            min = rssi;
            selected = std::string(msg->par("src"));
            EV << "==> MS[" << imsi << "] New Best!" << selected << endl;
        }
    }
    else if(std::string(msg->par("src"))==selected) // If the beacon is from the currently connected BTS
    {
        double rssi = getRSSIFromPacket(msg);
        min = rssi;
    }
    delete msg;
}

void MS::startScanning()
{
    scanStatus = true;
    min = 0;
    //scanChannelsStop = new cMessage( "STOP_SCANNING",STOP_SCANNING);// Sending connection request to new bts
    // When the self message is received, stop listening for beacons.
    scheduleAt(simTime()+1, scanChannelsStop);
}

void MS::stopScanning()
{
    scanStatus = false;
    EV << "==> MS[" << imsi << "] Stop looking for beacons" << endl;
    EV << "==> MS[" << imsi << "] The best BTS is "<< selected <<endl;
    lastBeaconUpdate = simTime();
    if(min == 0){
        selected = "no_bts";
        //RSSIstats.collect(0);
        //currentRSSI.record(0);
        emit(beaconRSSIsignal, 0);
    }
    else{
        //RSSIstats.collect(min);
        //currentRSSI.record(min);
        emit(beaconRSSIsignal, min);
    }
    //scanChannelsStart = new cMessage( "START_SCANNING",START_SCANNING);
    scheduleAt(simTime()+ beaconListenInterval,scanChannelsStart);
}

double MS::getRSSIFromPacket(cMessage *msg)
{
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



