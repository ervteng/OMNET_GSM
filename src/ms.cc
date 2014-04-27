//-------------------------------------------------------------
// file: ms.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"
#include "ms.h"
#include "GSMRadio.h"
#include "Radio80211aControlInfo_m.h"

Define_Module(MS);

MS::MS(){}

void MS::initialize(){
    status=TRIGGER;                            // program state                          // number of handovers
    delay=1.0;

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

    counter = simTime()+delay;
    lastBeaconUpdate = simTime();
    scheduleAt(simTime(),movecar);


    // FOR DEBUGGING ONLY
    callinterval = 0.1;
}
// Write the logs at the end of the simulation
void MS::finish()
{
    double  dblTemp;
    char    sTemp[128];
    FILE *out;

    out = fopen("statistic.txt","at");
    fprintf(out,"MS#%d Number of attempted calls: %d\n",imsi,iCalls);
    if (iCalls-iMissedCalls > 0)
        dblTemp = iHandover*100.0/(iCalls-iMissedCalls);
    else
        dblTemp = 0.0;
    sprintf(sTemp,"%.4lf",dblTemp);
    fprintf(out,"MS#%d Number of successful calls: %d, handover: %d, percentage: %s%%\n",imsi,iCalls-iMissedCalls,iHandover,sTemp);
    if (iCalls > 0)
        dblTemp = iMissedCalls*100.0/iCalls;
    else
        dblTemp = 100.0;
    sprintf(sTemp,"%.4lf",dblTemp);
    fprintf(out,"MS#%d Missed calls: %d, error percentage: %s%%\n",imsi,iMissedCalls,sTemp );
    if (iCalls > 0)
        dblTemp = iBroken*100.0/iCalls;
    else
        dblTemp = 100.0;
    sprintf(sTemp,"%.4lf",dblTemp);
    fprintf(out,"MS#%d Broken calls: %d, error percentage: %s%%\n",imsi,iBroken,sTemp );
    fclose(out);

    recordScalar("# Attempted Calls", iCalls);
    recordScalar("# Successful Calls", iCalls-iMissedCalls);
    recordScalar("# Handovers", iHandover);
    RSSIstats.recordAs("ReceivedRSSIs");
}

void MS::receiveChangeNotification(int category, const cObject *details)
{
    EV << "------------ receiveChangeNotification ----------\n";
    EV << category << endl;
}

void MS::handleMessage(cMessage *msg)
{
//    EV << "==> DEBUG: handleMessage: params\n";
//    int n = getNumParams();
//    for (int i=0; i<n; i++) {
//           cPar& p = par(i);
//           ev << "parameter: " << p.getName() << "\n";
//           ev << "  type:" << cPar::getTypeName(p.getType()) << "\n";
//           ev << "  contains:" << p.str() << "\n";
//    }
//    EV << "==> DEBUG: End params\n";

    type = msg->getKind();
    EV << "Message is " << type << "\n";
    if(msg->isSelfMessage()){
        if (type == TRIGGER){ // TODO Replace with selfmessage logic
            processMsgTrigger(msg);
        }
        else if(type == SCANNING){
            EV << "Stop looking for beacons" << endl;
            lastBeaconUpdate = simTime();
        }
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

        EV << "TIme to attempt a call! sending message to" << selected <<endl;
        if(strcmp(selected,"no_bts") == 0)
        {
            EV << "We have no reception here. Sucks."<<endl;
        }
        else{
            alltime=0;
            check_bts = new cPacket( "CHECK_BTS", CHECK_BTS );
            check_bts->addPar( *new cMsgPar("src")  = imsi );
            check_bts->addPar( *new cMsgPar("dest") = selected );
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
        disc_req->addPar("dest") = connected;
        send( disc_req, "to_air" );                // Send disconnect request to the BTS
        lastmsg=simTime();
        status=DISC_REQ;
    };
}


void MS::processMsgTrigger(cMessage *msg)
{

    alltime+=delay;
    movecar = new cPacket("TRIGGER",TRIGGER);// send new scheduled message
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
            if (strcmp(selected,"no_bts")!=0)                // there's an available BTS
            {
                ev << "==> MS[" << imsi << "] sending CONN_REQ to #" << selected << '\n';
                conn_req = new cPacket( "CONN_REQ",CONN_REQ);
                conn_req->addPar( *new cMsgPar("src") =imsi);
                conn_req->addPar( *new cMsgPar("dest")=selected);
                send( conn_req, "to_air" );    // Send connection request
                status=CONN_REQ;
                lastmsg=simTime();
                iCalls++;
            } else
            {                                // no available BTS
                iCalls++;
                iMissedCalls++;
                ev << "==> MS[" << imsi << "] Can't connect (CHECK_BTS). Error #" << iMissedCalls;
                // calculate new parameters
                callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
                calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
                status=TRIGGER;
                out = fopen("statistic.txt","at");// Log missed call
                //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),imsi);
                fclose(out);
            };
        };
        break;

    case CONN_REQ:                            // Waiting for connection ack
        if (lastmsg+timeout>=simTime())
        {
            if (strcmp(connected,"no_bts")!=0)                // If we were connected before
            {
                                            // Broken handover
                iBroken++;
                out = fopen("statistic.txt","at");// Log broken call
                //fprintf(out,"At %s = MS#%d Broken call\n",simtimeToStr(lastmsg),imsi);
                fclose(out);
            }
            else
            {
                                            // Can't make a new call
                iMissedCalls++;
                out = fopen("statistic.txt","at");// Log missed call
                //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),imsi);
                fclose(out);
            }
            ev << "==> MS[" << imsi << "] Can't connect (CONN_REQ). Error #" << iMissedCalls;
                                              // Calculate new call parameters
            callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
            calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
            status=TRIGGER;
        };
        break;

    case CONN_ACK:                            // Check for handover periodically
        check_line = new cPacket("CHECK_LINE",CHECK_LINE);
        check_line->addPar( *new cMsgPar("src") = imsi);
        check_line->addPar( *new cMsgPar("dest") = connected );
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
    if (rssi > min)                        // More then the best
    {
        min=rssi;
        selected=msg->par("src");
        currentRSSI.record(min);
        RSSIstats.collect(min);
    };
//    if (i < num_bts-1){                        // If not all BTS checked
//        i++;
//        check_bts = new cPacket("CHECK_BTS",CHECK_BTS);
//        check_bts->addPar( *new cMsgPar("src") = imsi);
//        check_bts->addPar( *new cMsgPar("dest") = i );
//        ev << "==> MS[" << imsi << "] Sending CHECK_BTS to #" << i << '\n';
//        send(check_bts,"to_air");            // Send check_bts to the next BTS
//        lastmsg=simTime();                    // Set the time of last msg
    if (strcmp(selected,"no_bts")!=0) {                       // We have a good BTS, let's connect
        ev << "==> MS[" << imsi << "] sending CONN_REQ to #" << selected << '\n';
        conn_req = new cPacket( "CONN_REQ",CONN_REQ);
        conn_req->addPar( *new cMsgPar("src") = imsi);
        conn_req->addPar( *new cMsgPar("dest") = selected);
        send( conn_req, "to_air" );            // Send connection request
        status=CONN_REQ;
        lastmsg=simTime();
        iCalls++;
    } else {                                        // No answer from BTSs
        iCalls++;
        iMissedCalls++;
        ev << "Can't connect. (CHECK_BTS) Error #" << iMissedCalls;
                                            // Calculate new call parameters
        callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
        calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
        status=TRIGGER;
        out = fopen("statistic.txt","at");    // Log missed call
        //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),imsi);
        fclose(out);
    }
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
    iBroken++;
    out = fopen("statistic.txt","at");        // Log broken call
    //fprintf(out,"At %s = MS#%d Broken call\n",simtimeToStr(lastmsg),imsi);
    fclose(out);
}

void MS::processMsgCheckMs(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got CHECK_MS\n";
    check_ms = new cPacket( "MS_DATA",MS_DATA);
    check_ms->addPar( *new cMsgPar("src") = imsi);
    check_ms->addPar( *new cMsgPar("dest") = msg->par("src"));
    send( check_ms, "to_air" );                // Send back the current position
}

void MS::processMsgHandoverMs(cMessage *msg)
{
    ev << "==> MS[" << imsi << "] got HANDOVER_MS \n";
    if (strcmp(connected,"no_bts") != 0){
        ev << "got HANDOVER_MS \n";
        selected = msg->par("newbts");
        ev << "sending CONN_REQ to BCC " << selected << '\n';
        conn_req = new cPacket( "CONN_REQ",CONN_REQ);// Sending connection request to new bts
        conn_req->addPar( *new cMsgPar("src") = imsi);
        conn_req->addPar( *new cMsgPar("dest") = selected);
        send( conn_req, "to_air" );
        status=CONN_REQ;
        lastmsg=simTime();
        iHandover++;
        out = fopen("statistic.txt","at");    // Log handover
        //fprintf(out,"At %s = MS#%d HANDOVER from %d to %d\n",simtimeToStr(lastmsg),imsi,connected,selected);
        fclose(out);
    }
}

void MS::processMsgBtsBeacon(cMessage *msg)
{

    if(simTime() > (lastBeaconUpdate + beaconListenInterval) && status == TRIGGER )
    {// if the last time we updated from a beacon was too long ago
        min = 0;
        ev << "==> MS[" << imsi << "] got BEACON\n";
        double rssi = getRSSIFromPacket(msg);
        EV << "==> MS[" << imsi << "] RCV Rssi=" << rssi << endl;
        if(rssi > min){
            min = rssi;
            selected = msg->par("src");

            //Record new RSSI
            currentRSSI.record(min);
            RSSIstats.collect(min);
        }
    }
    cMessage * scanChannels = new cMessage( "SCANNING",SCANNING);// Sending connection request to new bts
    // When the self message is received, stop listening for beacons.
    scheduleAt(simTime()+1, scanChannels);
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


