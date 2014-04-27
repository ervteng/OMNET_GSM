/*
 * bsc.h
 *
 *  Created on: Apr 14, 2014
 *      Author: Lock
 */

#ifndef BSC_H_
#define BSC_H_
#include <map>

// The BSC object
class BSC: public cSimpleModule {
    //Module_Class_Members(BSC,cSimpleModule,16384);
    public:
        BSC();
    protected:
        virtual void initialize();
//        virtual void activity();
        virtual void destroy();
        virtual void handleMessage(cMessage *msg);
        virtual void processHandoverEnd(cMessage *msg);
        virtual void processHandoverCheck(cMessage *msg);
        virtual void processHandoverData(cMessage *msg);
        virtual void buildRoutingTable();
        virtual void sendToBTS(cMessage *msg, const char* BCC);

    private:
        double *iWatts;              // Buffer to hold the max. watts for the phones
        int *iBTS;                          // Buffer to hold the bts for the phones
        double delay;
        int num_bts;
        int i;
        bool isRouted;
        std::map<std::string, double> routingTable;
        std::map<std::string, std::string> MStoBTS;
        std::map<std::string, double> MSRSSI;
};



#endif /* BSC_H_ */
