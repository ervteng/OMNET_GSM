/*
 * bts.h
 *
 *  Created on: Apr 9, 2014
 *      Author: Lock
 */

#ifndef BTS_H_
#define BTS_H_

#include <omnetpp.h>

// The BTS object
class BTS: public cSimpleModule {
    //Module_Class_Members(BTS,cSimpleModule,16384);
    public:
        BTS();

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
//        virtual void activity();
        virtual void destroy();

    private:
        double calculateWatt(double dblMSXc, double dblMSYc); // Calculate current watt
        double calculateWatt(double dblMSXc, double dblMSYc, double recPower); // Calculate current watt
        int iSlots;                                        // How many connection can hold the bts
        int iPhones;                                       // Number of phones
        int iConnections;
        int *iPhoneState;                                 // Buffer to hold the phone states
        double dblXc;                                     // X coordinate
        double dblYc;                                     // Y coordinate
        double dblRadius;                                 // Radius
        double dblWatt;                                   // Watt
};




#endif /* BTS_H_ */
