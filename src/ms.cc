//-------------------------------------------------------------
// file: ms.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"
#include "ms.h"

Define_Module(MS);

MS::MS(){}

void MS::initialize(){
    status=MOVE_CAR;                            // program state                          // number of handovers
    delay=1.0;
    // Query module parameters
    xc = (double) par( "xc" );
    yc = (double) par( "yc" );
    vx = par( "vx" );
    vy = par( "vy" );
    connected = -1;
    callinterval = CALL_INTERVAL_MIN + exponential(CALL_INTERVAL);
    calllength = CALL_LENGTH_MIN + exponential(CALL_LENGTH);
    timeout = par ( "timeout" );

    // assign address: index of gate to which we are connected
    own_addr = gate( "to_air" )->getNextGate()->getIndex();
    selected=0;
    num_bts = 0;

    movecar = new cPacket("MOVE_CAR",MOVE_CAR);    // send the first scheduled move message
    //SimTime counter = simTime()+delay;

    nextCall = new cPacket("make_call");
    scheduleAt(callinterval, nextCall);

    counter = simTime()+delay;
    scheduleAt(simTime(),movecar);
    callinterval = 0.1;
}
// Write the logs at the end of the simulation
void MS::finish()
{
    double  dblTemp;
    char    sTemp[128];
    FILE *out;

    out = fopen("statistic.txt","at");
    fprintf(out,"MS#%d Number of attempted calls: %d\n",own_addr,iCalls);
    if (iCalls-iMissedCalls > 0)
        dblTemp = iHandover*100.0/(iCalls-iMissedCalls);
    else
        dblTemp = 0.0;
    sprintf(sTemp,"%.4lf",dblTemp);
    fprintf(out,"MS#%d Number of successful calls: %d, handover: %d, percentage: %s%%\n",own_addr,iCalls-iMissedCalls,iHandover,sTemp);
    if (iCalls > 0)
        dblTemp = iMissedCalls*100.0/iCalls;
    else
        dblTemp = 100.0;
    sprintf(sTemp,"%.4lf",dblTemp);
    fprintf(out,"MS#%d Missed calls: %d, error percentage: %s%%\n",own_addr,iMissedCalls,sTemp );
    if (iCalls > 0)
        dblTemp = iBroken*100.0/iCalls;
    else
        dblTemp = 100.0;
    sprintf(sTemp,"%.4lf",dblTemp);
    fprintf(out,"MS#%d Broken calls: %d, error percentage: %s%%\n",own_addr,iBroken,sTemp );
    fclose(out);
}

void MS::receiveChangeNotification(int category, const cObject *details)
{
    EV << "------------ receiveChangeNotification ----------\n";
    EV << category << endl;
}


void MS::processMsgMoveCar(cMessage *msg)
{
    xc+=(double) delay*vx;                    // new posx=delay*vx
    yc+=(double) delay*vy;                    // new posy=delay*vy
    alltime+=delay;
    movecar = new cPacket("MOVE_CAR",MOVE_CAR);// send new scheduled message
    counter = simTime()+delay;
    scheduleAt(counter,movecar);
    switch (status){
    case CHECK_BTS:                            // check if timeout happened
        if (lastmsg+timeout>=simTime())
        {
            if (i<num_bts-1)                // if not all BTS checked
            {
                i++;                        // send new check message
                check_bts=new cPacket("CHECK_BTS",CHECK_BTS);
                check_bts->addPar( *new cMsgPar("src") =own_addr);
                check_bts->addPar( *new cMsgPar("xc") =xc);
                check_bts->addPar( *new cMsgPar("yc") =yc);
                check_bts->addPar( *new cMsgPar("dest") =i);
                ev << "Sending CHECK_BTS to #" << i << '\n';
                send(check_bts,"to_air");
                lastmsg=simTime();
            } else
            if (selected>-1)                // there's an available BTS
            {
                ev << "sending CONN_REQ to #" << i << '\n';
                conn_req = new cPacket( "CONN_REQ",CONN_REQ);
                conn_req->addPar( *new cMsgPar("src") =own_addr);
                conn_req->addPar( *new cMsgPar("dest")=selected);
                send( conn_req, "to_air" );    // Send connection request
                status=CONN_REQ;
                lastmsg=simTime();
                iCalls++;
            } else
            {                                // no available BTS
                iCalls++;
                iMissedCalls++;
                ev << "Can't connect (CHECK_BTS). Error #" << iMissedCalls;
                // calculate new parameters
                callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
                calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
                status=MOVE_CAR;
                out = fopen("statistic.txt","at");// Log missed call
                //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),own_addr);
                fclose(out);
            };
        };
        break;

    case CONN_REQ:                            // Waiting for connection ack
        if (lastmsg+timeout>=simTime())
        {
            if (connected > -1)                // If we were connected before
            {
                                            // Broken handover
                iBroken++;
                out = fopen("statistic.txt","at");// Log broken call
                //fprintf(out,"At %s = MS#%d Broken call\n",simtimeToStr(lastmsg),own_addr);
                fclose(out);
            }
            else
            {
                                            // Can't make a new call
                iMissedCalls++;
                out = fopen("statistic.txt","at");// Log missed call
                //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),own_addr);
                fclose(out);
            }
            ev << "Can't connect (CONN_REQ). Error #" << iMissedCalls;
                                              // Calculate new call parameters
            callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
            calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
            status=MOVE_CAR;
        };
        break;

    case CONN_ACK:                            // Check for handover periodically
        check_line = new cPacket("CHECK_LINE",CHECK_LINE);
        check_line->addPar( *new cMsgPar("src") = own_addr);
        check_line->addPar( *new cMsgPar("xc")  = xc );
        check_line->addPar( *new cMsgPar("yc")  = yc );
        check_line->addPar( *new cMsgPar("dest") = connected );
        ev << "Sending CHECK_LINE to #" << connected << '\n';
        send(check_line,"to_air");            // Sending CHECK_LINE message to BTS
        break;
    };

}

void MS::processMsgBtsData(cMessage *msg)
{
    ev << "got BTS_DATA\n";
    dblTemp = msg->par("data");            // Watt
    if (dblTemp > min)                        // More then the best
    {
        min=msg->par("data");
        selected=msg->par("src");
    };
    if (i < num_bts-1){                        // If not all BTS checked
        i++;
        check_bts = new cPacket("CHECK_BTS",CHECK_BTS);
        check_bts->addPar( *new cMsgPar("src") = own_addr);
        check_bts->addPar( *new cMsgPar("xc")  = xc );
        check_bts->addPar( *new cMsgPar("yc")  = yc );
        check_bts->addPar( *new cMsgPar("dest") = i );
        ev << "Sending CHECK_BTS to #" << i << '\n';
        send(check_bts,"to_air");            // Send check_bts to the next BTS
        lastmsg=simTime();                    // Set the time of last msg
    } else if (selected > -1) {                       // We have a good BTS, let's connect
        ev << "sending CONN_REQ to #" << i << '\n';
        conn_req = new cPacket( "CONN_REQ",CONN_REQ);
        conn_req->addPar( *new cMsgPar("src") = own_addr);
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
        status=MOVE_CAR;
        out = fopen("statistic.txt","at");    // Log missed call
        //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),own_addr);
        fclose(out);
    }
}

void MS::processMsgConnAck(cMessage *msg)
{
    ev << "got CONN_ACK\n";
    i=msg->par("src");
    connected = selected;                    // Connected to this BTS
    ev << "Talking with station #" << i << '\n';
    status=CONN_ACK;
    lastmsg=simTime();
    alltime=0;                                // Reset the timer
}

void MS::processMsgDiscAck(cMessage *msg)
{
    ev << "got DISC_ACK\n";
    lastmsg=simTime();
    alltime=0;
    status=MOVE_CAR;
                                            // Calculate new call parameters
    callinterval = CALL_INTERVAL + exponential(CALL_INTERVAL_MIN);
    calllength = CALL_LENGTH_MIN + exponential(CALL_LENGTH);
    connected = -1;
}

void MS::processMsgForceDisc(cMessage *msg)
{
    ev << "got FORCE_DISC\n";
    lastmsg=simTime();
    alltime=0;
    status=MOVE_CAR;
                                            // Calculate new call parameters
    callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
    calllength = CALL_RETRY_LENGTH_MIN /2 + exponential(CALL_RETRY_LENGTH) /2;
    connected = -1;                            // Not connected to a BTS
    iBroken++;
    out = fopen("statistic.txt","at");        // Log broken call
    //fprintf(out,"At %s = MS#%d Broken call\n",simtimeToStr(lastmsg),own_addr);
    fclose(out);
}

void MS::processMsgCheckMs(cMessage *msg)
{
    ev << "got CHECK_MS\n";
    check_ms = new cPacket( "MS_DATA",MS_DATA);
    check_ms->addPar( *new cMsgPar("src") = own_addr);
    check_ms->addPar( *new cMsgPar("dest") = msg->par("src"));
    check_ms->addPar( *new cMsgPar("xc")  = xc );
    check_ms->addPar( *new cMsgPar("yc")  = yc );
    send( check_ms, "to_air" );                // Send back the current position
}

void MS::processMsgHandoverMs(cMessage *msg)
{
    ev << "got HANDOVER_MS \n";
    if (connected > -1){
        ev << "got HANDOVER_MS \n";
        selected = msg->par("newbts");
        ev << "sending CONN_REQ to #" << selected << '\n';
        conn_req = new cPacket( "CONN_REQ",CONN_REQ);// Sending connection request to new bts
        conn_req->addPar( *new cMsgPar("src") = own_addr);
        conn_req->addPar( *new cMsgPar("dest") = selected);
        send( conn_req, "to_air" );
        status=CONN_REQ;
        lastmsg=simTime();
        iHandover++;
        out = fopen("statistic.txt","at");    // Log handover
        //fprintf(out,"At %s = MS#%d HANDOVER from %d to %d\n",simtimeToStr(lastmsg),own_addr,connected,selected);
        fclose(out);
    }
}

void MS::handleMessage(cMessage *msg)
{
    EV << "==> DEBUG: handleMessage: params\n";
    int n = getNumParams();
    for (int i=0; i<n; i++) {
           cPar& p = par(i);
           ev << "parameter: " << p.getName() << "\n";
           ev << "  type:" << cPar::getTypeName(p.getType()) << "\n";
           ev << "  contains:" << p.str() << "\n";
    }
    EV << "==> DEBUG: End params\n";

    type = msg->getKind();
    if (type == MOVE_CAR){ // TODO Replace with selfmessage logic
        processMsgMoveCar(msg);
    }else if (type == BTS_DATA){
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
        EV <<  "ERROR: no matched message type=" << type;
    }


    if ((alltime>=(double)callinterval)&&(status==MOVE_CAR)) {
        alltime=0;
                                                // Search for the best station
        min=0.0;selected=-1;
        i=0;
        check_bts = new cPacket( "CHECK_BTS", CHECK_BTS );
        check_bts->addPar( *new cMsgPar("src")  = own_addr );
        check_bts->addPar( *new cMsgPar("xc")  = xc );
        check_bts->addPar( *new cMsgPar("yc")  = yc );
        check_bts->addPar( *new cMsgPar("dest") = i );
        ev << "Sending CHECK_BTS to #" << i << '\n';
        send( check_bts, "to_air" );            // Send the first BTS check message
        status=CHECK_BTS;                        // New state
        lastmsg=simTime();
    };

    // If it's due then terminate the call
    if ((alltime>=(double)calllength)&&(status==CONN_ACK)) {
        ev << "sending DISC_REQ\n";
        disc_req = new cPacket( "DISC_REQ", DISC_REQ );
        disc_req->addPar("src") = own_addr;
        disc_req->addPar("dest") = connected;
        send( disc_req, "to_air" );                // Send disconnect request to the BTS
        lastmsg=simTime();
        status=DISC_REQ;
    };
}

//void MS::handleMessage(cMessage *msg)
//{
//
//    EV << "<<<<+++++++++++++++++++++++++>>>>\n";
//    if (msg->isSelfMessage()){
//       EV << "Got self message!!!!!!!\n";
//    }else{
//       EV << "+++++++++++++++++++++++++\n";
//    }
//
////    int n = getNumParams();
////    for (int i=0; i<n; i++) {
////           cPar& p = par(i);
////           ev << "parameter: " << p.getName() << "\n";
////           ev << "  type:" << cPar::getTypeName(p.getType()) << "\n";
////           ev << "  contains:" << p.str() << "\n";
////    }
//
////    cModule *mobililty = this->getParentModule()->getSubmodule("mobility");
////    int xx = mobililty->par("speed");
////    EV << xx << "----------========\n";
////
////    LinearMobility *lm = (LinearMobility *)mobililty;
////    Coord co = lm->getCurrentPosition();
////    EV << co.x << "," << co.y << "----------========\n";
//
////    type = msg->getKind();
////    switch (type)
////        {
////        case MOVE_CAR:
////            xc+=(double) delay*vx;                    // new posx=delay*vx
////            yc+=(double) delay*vy;                    // new posy=delay*vy
////            alltime+=delay;
////            movecar = new cPacket("MOVE_CAR",MOVE_CAR);// send new scheduled message
////            counter = simTime()+delay;
////            scheduleAt(counter,movecar);
////            switch (status)
////            {
////            case CHECK_BTS:                            // check if timeout happened
////                if (lastmsg+timeout>=simTime())
////                {
////                    if (i<num_bts-1)                // if not all BTS checked
////                    {
////                        i++;                        // send new check message
////                        check_bts=new cPacket("CHECK_BTS",CHECK_BTS);
////                        check_bts->addPar( *new cMsgPar("src") =own_addr);
////                        check_bts->addPar( *new cMsgPar("xc") =xc);
////                        check_bts->addPar( *new cMsgPar("yc") =yc);
////                        check_bts->addPar( *new cMsgPar("dest") =i);
////                        ev << "Sending CHECK_BTS to #" << i << '\n';
////                        send(check_bts,"to_air");
////                        lastmsg=simTime();
////                    } else
////                    if (selected>-1)                // there's an available BTS
////                    {
////                        ev << "sending CONN_REQ to #" << i << '\n';
////                        conn_req = new cPacket( "CONN_REQ",CONN_REQ);
////                        conn_req->addPar( *new cMsgPar("src") =own_addr);
////                        conn_req->addPar( *new cMsgPar("dest")=selected);
////                        send( conn_req, "to_air" );    // Send connection request
////                        status=CONN_REQ;
////                        lastmsg=simTime();
////                        iCalls++;
////                    } else
////                    {                                // no available BTS
////                        iCalls++;
////                        iMissedCalls++;
////                        ev << "Can't connect (CHECK_BTS). Error #" << iMissedCalls;
////                        // calculate new parameters
////                        callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
////                        calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
////                        status=MOVE_CAR;
////                        out = fopen("statistic.txt","at");// Log missed call
////                        //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),own_addr);
////                        fclose(out);
////                    };
////                };
////                break;
////
////            case CONN_REQ:                            // Waiting for connection ack
////                if (lastmsg+timeout>=simTime())
////                {
////                    if (connected > -1)                // If we were connected before
////                    {
////                                                    // Broken handover
////                        iBroken++;
////                        out = fopen("statistic.txt","at");// Log broken call
////                        //fprintf(out,"At %s = MS#%d Broken call\n",simtimeToStr(lastmsg),own_addr);
////                        fclose(out);
////                    }
////                    else
////                    {
////                                                    // Can't make a new call
////                        iMissedCalls++;
////                        out = fopen("statistic.txt","at");// Log missed call
////                        //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),own_addr);
////                        fclose(out);
////                    }
////                    ev << "Can't connect (CONN_REQ). Error #" << iMissedCalls;
////                                                      // Calculate new call parameters
////                    callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
////                    calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
////                    status=MOVE_CAR;
////                };
////                break;
////
////            case CONN_ACK:                            // Check for handover periodically
////                check_line = new cPacket("CHECK_LINE",CHECK_LINE);
////                check_line->addPar( *new cMsgPar("src") = own_addr);
////                check_line->addPar( *new cMsgPar("xc")  = xc );
////                check_line->addPar( *new cMsgPar("yc")  = yc );
////                check_line->addPar( *new cMsgPar("dest") = connected );
////                ev << "Sending CHECK_LINE to #" << connected << '\n';
////                send(check_line,"to_air");            // Sending CHECK_LINE message to BTS
////                break;
////            };
////            break;
////
////        // First case ends
////
////        case BTS_DATA:                                // Information from a BTS
////            dblTemp = msg->par("data");            // Watt
////            if (dblTemp > min)                        // More then the best
////            {
////                min=msg->par("data");
////                selected=msg->par("src");
////            };
////            if (i < num_bts-1)                        // If not all BTS checked
////            {
////                i++;
////                check_bts = new cPacket("CHECK_BTS",CHECK_BTS);
////                check_bts->addPar( *new cMsgPar("src") = own_addr);
////                check_bts->addPar( *new cMsgPar("xc")  = xc );
////                check_bts->addPar( *new cMsgPar("yc")  = yc );
////                check_bts->addPar( *new cMsgPar("dest") = i );
////                ev << "Sending CHECK_BTS to #" << i << '\n';
////                send(check_bts,"to_air");            // Send check_bts to the next BTS
////                lastmsg=simTime();                    // Set the time of last msg
////            } else
////            if (selected > -1)                        // We have a good BTS, let's connect
////            {
////                ev << "sending CONN_REQ to #" << i << '\n';
////                conn_req = new cPacket( "CONN_REQ",CONN_REQ);
////                conn_req->addPar( *new cMsgPar("src") = own_addr);
////                conn_req->addPar( *new cMsgPar("dest") = selected);
////                send( conn_req, "to_air" );            // Send connection request
////                status=CONN_REQ;
////                lastmsg=simTime();
////                iCalls++;
////            } else
////            {                                        // No answer from BTSs
////                iCalls++;
////                iMissedCalls++;
////                ev << "Can't connect. (CHECK_BTS) Error #" << iMissedCalls;
////                                                    // Calculate new call parameters
////                callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
////                calllength = CALL_RETRY_LENGTH_MIN + exponential(CALL_RETRY_LENGTH);
////                status=MOVE_CAR;
////                out = fopen("statistic.txt","at");    // Log missed call
////                //fprintf(out,"At %s = MS#%d Missed call\n",simtimeToStr(lastmsg),own_addr);
////                fclose(out);
////            }
////            break;
////
////        case CONN_ACK:                                // Connection acknowledgement from BTS
////            ev << "got CONN_ACK\n";
////            i=msg->par("src");
////            connected = selected;                    // Connected to this BTS
////            ev << "Talking with station #" << i << '\n';
////            status=CONN_ACK;
////            lastmsg=simTime();
////            alltime=0;                                // Reset the timer
////            break;
////
////            // timeout error not checked!!!!!!!!!!!!!!
////        case DISC_ACK:                                // Disconnect acknowledgement from BTS
////            ev << "got DISC_ACK\n";
////            lastmsg=simTime();
////            alltime=0;
////            status=MOVE_CAR;
////                                                    // Calculate new call parameters
////            callinterval = CALL_INTERVAL + exponential(CALL_INTERVAL_MIN);
////            calllength = CALL_LENGTH_MIN + exponential(CALL_LENGTH);
////            connected = -1;                            // Not connected to a BTS
////            break;
////
////        case FORCE_DISC:                            // Disconnect from BTS
////            ev << "got FORCE_DISC\n";
////            lastmsg=simTime();
////            alltime=0;
////            status=MOVE_CAR;
////                                                    // Calculate new call parameters
////            callinterval = CALL_RETRY_INTERVAL_MIN + exponential(CALL_RETRY_INTERVAL);
////            calllength = CALL_RETRY_LENGTH_MIN /2 + exponential(CALL_RETRY_LENGTH) /2;
////            connected = -1;                            // Not connected to a BTS
////            iBroken++;
////            out = fopen("statistic.txt","at");        // Log broken call
////            //fprintf(out,"At %s = MS#%d Broken call\n",simtimeToStr(lastmsg),own_addr);
////            fclose(out);
////            break;
////
////        case CHECK_MS:                                // Check MS for handover
////            ev << "got CHECK_MS\n";
////            check_ms = new cPacket( "MS_DATA",MS_DATA);
////            check_ms->addPar( *new cMsgPar("src") = own_addr);
////            check_ms->addPar( *new cMsgPar("dest") = msg->par("src"));
////            check_ms->addPar( *new cMsgPar("xc")  = xc );
////            check_ms->addPar( *new cMsgPar("yc")  = yc );
////            send( check_ms, "to_air" );                // Send back the current position
////            break;
////
////        case HANDOVER_MS:                            // Handover request from BTS
////            if (connected > -1)
////            {
////                ev << "got HANDOVER_MS \n";
////                selected = msg->par("newbts");
////                ev << "sending CONN_REQ to #" << selected << '\n';
////                conn_req = new cPacket( "CONN_REQ",CONN_REQ);// Sending connection request to new bts
////                conn_req->addPar( *new cMsgPar("src") = own_addr);
////                conn_req->addPar( *new cMsgPar("dest") = selected);
////                send( conn_req, "to_air" );
////                status=CONN_REQ;
////                lastmsg=simTime();
////                iHandover++;
////                out = fopen("statistic.txt","at");    // Log handover
////                //fprintf(out,"At %s = MS#%d HANDOVER from %d to %d\n",simtimeToStr(lastmsg),own_addr,connected,selected);
////                fclose(out);
////            }
////            break;
////        }
//
////        if ((alltime>=(double)callinterval)&&(status==MOVE_CAR))
////        {
////            alltime=0;
////                                                    // Search for the best station
////            min=0.0;selected=-1;
////            i=0;
////            check_bts = new cPacket( "CHECK_BTS", CHECK_BTS );
////            check_bts->addPar( *new cMsgPar("src")  = own_addr );
////            check_bts->addPar( *new cMsgPar("xc")  = xc );
////            check_bts->addPar( *new cMsgPar("yc")  = yc );
////            check_bts->addPar( *new cMsgPar("dest") = i );
////            ev << "Sending CHECK_BTS to #" << i << '\n';
////            send( check_bts, "to_air" );            // Send the first BTS check message
////            status=CHECK_BTS;                        // New state
////            lastmsg=simTime();
////        };
////
////        // If it's due then terminate the call
////        if ((alltime>=(double)calllength)&&(status==CONN_ACK))
////        {
////            ev << "sending DISC_REQ\n";
////            disc_req = new cPacket( "DISC_REQ", DISC_REQ );
////            disc_req->addPar("src") = own_addr;
////            disc_req->addPar("dest") = connected;
////            send( disc_req, "to_air" );                // Send disconnect request to the BTS
////            lastmsg=simTime();
////            status=DISC_REQ;
////        };
//}
