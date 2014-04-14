/*
 * bsc.h
 *
 *  Created on: Apr 14, 2014
 *      Author: Lock
 */

#ifndef BSC_H_
#define BSC_H_

// The BSC object
class Bsc: public cSimpleModule {
    //Module_Class_Members(BSC,cSimpleModule,16384);
    public:
        Bsc();
    protected:
        virtual void initialize();
//        virtual void activity();
        virtual void destroy();
        virtual void handleMessage(cMessage *msg);
        virtual void processHandoverEnd(cMessage *msg);
        virtual void processHandoverCheck(cMessage *msg);
        virtual void processHandoverData(cMessage *msg);

    private:
        double *iWatts;              // Buffer to hold the max. watts for the phones
        int *iBTS;                          // Buffer to hold the bts for the phones
        double delay;
        int num_bts;
};



#endif /* BSC_H_ */
