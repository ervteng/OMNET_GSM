/*
 * bts.h
 *
 *  Created on: Apr 9, 2014
 *      Author: Lock
 */

#ifndef BTS_H_
#define BTS_H_

#include <omnetpp.h>
#include <set>

// The BTS object
class BTS: public cSimpleModule {
    //Module_Class_Members(BTS,cSimpleModule,16384);
    public:
        BTS();
        const char* bcc;                                     //Address of the BTS

    protected:
        virtual void initialize();
        virtual void finish();
        virtual void handleMessage(cMessage *msg);
        virtual void processMsgConnReqFromMs(cMessage *msg);
        virtual void processMsgCheckBtsFromMs(cMessage *msg);
        virtual void processMsgDiscReqFromMs(cMessage *msg);
        virtual void processMsgHandoverFromBsc(cMessage *msg);
        virtual void processMsgForceCheckMsFromBsc(cMessage *msg);
        virtual void processMsgDataFromMs(cMessage *msg);
        virtual void processMsgCheckLineFromMs(cMessage *msg);
        virtual void sendBeacon();
        //virtual double getRssiFromRadio();
//        virtual void activity();
        virtual void destroy();


    private:
        double calculateWatt(double dblMSXc, double dblMSYc); // Calculate current watt
        double getRSSIFromPacket(cMessage *msg); // Calculate current watt using recPower
        int iSlots;                                        // How many connection can hold the bts
        int iPhones;                                       // Number of phones
        int iConnections;
        int *iPhoneState;                                 // Buffer to hold the phone states
        double dblXc;                                     // X coordinate
        double dblYc;                                     // Y coordinate
        double dblRadius;                                 // Radius
        double dblWatt;                                   // Watt
        std::set<std::string> connectedPhones;                   //Set of connected phones
        double beaconInterval;
        cMessage *beaconTrigger;
        cPacket *bts_beacon;
        //Messages that go to the BSC
        cMessage *handover_chk, *handover_data;
        //Messages that go over the air
        cPacket *handover_ms, *force_disc;
        //Is the BTS on? Only the Malicious BTS is ever off
        bool isOn;

        // Signal
        simsignal_t connReqFromMsSignal;
        simsignal_t sendBeaconSignal;
        simsignal_t checkLineFromMsSignal;
};




#endif /* BTS_H_ */
