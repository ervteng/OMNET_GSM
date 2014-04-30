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
        int missedCalls;               // number of missed calls
        int attemptedCalls;                     // number of calls
        int brokenCalls;                    // number of broken calls
        int handOverCalls;               // number of handovers
        int successfullCalls;               // number of successful calls
        std::string connected;                  // BTS number if connected
        int status;                     // Finite state machine
        int type;                       // message type                          // program state                          // number of handovers
        double delay;
        double callinterval;
        double calllength;
        double timeout;


        cPacket *conn_req,*disc_req,*allmsg;
        cPacket *check_bts,*check_ms,*check_line;
        std::string selected;
        int num_bts;
        const char* imsi;
        SimTime lastmsg;
        double min;
        double beaconListenInterval;
        double scanDuration;
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

        // Signal
        simsignal_t attemptedCallSignal;
        simsignal_t successfulCallSignal;
        simsignal_t missedCallSignal;
        simsignal_t brokenCallSignal;
        simsignal_t handoverCallSignal;
        simsignal_t disconnectedCallSignal;

};


#endif /* MS_H_ */
