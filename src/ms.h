/*
 * ms.h
 *
 *  Created on: Apr 18, 2014
 *      Author: Lock
 */

#ifndef MS_H_
#define MS_H_

#include <omnetpp.h>
#include "simtime.h"
#include "LinearMobility.h"
#include "Coord.h"

class MS : public cSimpleModule, public INotifiable
{
    //Module_Class_Members(MS,cSimpleModule,16384)
    public:
        MS();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
        virtual void receiveChangeNotification(int category, const cObject *details);
        virtual void processMsgMoveCar(cMessage *msg); // self message
        virtual void processMsgBtsData(cMessage *msg); // Information from a BTS
        virtual void processMsgConnAck(cMessage *msg); // Connection acknowledgement from BTS
        virtual void processMsgDiscAck(cMessage *msg); // Disconnect acknowledgement from BTS
        virtual void processMsgForceDisc(cMessage *msg); // Disconnect from BTS
        virtual void processMsgCheckMs(cMessage *msg); // Check MS for handover
        virtual void processMsgHandoverMs(cMessage *msg); // Handover request from BTS

    private:
        int iMissedCalls;               // number of missed calls
        int iCalls;                     // number of calls
        int iBroken;                    // number of broken calls
        int iHandover;               // number of handovers
        int own_addr;                   // index of gate to which we are connected
        int connected;                  // BTS number if connected
        int status;                     // Finite state machine
        int i;                        // the number of bts
        int type;                       // message type                          // program state                          // number of handovers
        double delay;
        double dblTemp;
        // Query module parameters
        double xc;
        double yc;
        int vx;
        int vy;
        double callinterval;
        double calllength;
        double timeout;
        FILE *out;

        cMessage *conn_req,*disc_req,*movecar,*allmsg;
        cMessage *check_bts,*check_ms,*check_line;
        int selected;
        int num_bts;
        SimTime lastmsg;
        double min;
        SimTime counter;
        SimTime alltime;
        cMessage *nextCall;

};


#endif /* MS_H_ */
