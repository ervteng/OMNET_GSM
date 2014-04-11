/*
 * air.h
 *
 *  Created on: Apr 10, 2014
 *      Author: Lock
 */

#ifndef AIR_H_
#define AIR_H_


class Air: public cSimpleModule {
    public:
        Air();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void activity();
};


#endif /* AIR_H_ */
