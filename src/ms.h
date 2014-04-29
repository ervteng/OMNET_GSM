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
#include "string.h"

class MS : public cSimpleModule, public INotifiable
{
    //Module_Class_Members(MS,cSimpleModule,16384)
    public:
        MS();
        ~MS();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
        virtual void receiveChangeNotification(int category, const cObject *details);
        virtual void processMsgTrigger(cMessage *msg); // self message
        virtual void processMsgBtsData(cMessage *msg); // Information from a BTS
        virtual void processMsgConnAck(cMessage *msg); // Connection acknowledgement from BTS
        virtual void processMsgDiscAck(cMessage *msg); // Disconnect acknowledgement from BTS
        virtual void processMsgForceDisc(cMessage *msg); // Disconnect from BTS
        virtual void processMsgCheckMs(cMessage *msg); // Check MS for handover
        virtual void processMsgHandoverMs(cMessage *msg); // Handover request from BTS
        virtual void processMsgBtsBeacon(cMessage *msg);
        virtual double getRSSIFromPacket(cMessage *msg);
        virtual void stopScanning();
        virtual void startScanning();


    private:
        int iMissedCalls;               // number of missed calls
        int iCalls;                     // number of calls
        int iBroken;                    // number of broken calls
        int iHandover;               // number of handovers
        std::string connected;                  // BTS number if connected
        int status;                     // Finite state machine
        int i;                        // the number of bts
        int type;                       // message type                          // program state                          // number of handovers
        double delay;
        double dblTemp;
//        // Query module parameters
//        double xc;
//        double yc;
//        int vx;
//        int vy;
        double callinterval;
        double calllength;
        double timeout;
        FILE *out;

        cPacket *conn_req,*disc_req,*allmsg;
        cPacket *check_bts,*check_ms,*check_line;
        std::string selected;
        int num_bts;
        const char* imsi;
        SimTime lastmsg;
        double min;
        double beaconListenInterval;
        SimTime lastBeaconUpdate;
        SimTime counter;
        SimTime alltime;

        // Beginning trigger messages
        cMessage *nextCall, *movecar;
        cMessage *scanChannelsStart, *scanChannelsStop;

        //Instrumentation vectors
        simsignal_t beaconRSSIsignal;
        //cLongHistogram RSSIstats;
        //cOutVector currentRSSI;

        bool scanStatus;

};


#endif /* MS_H_ */
