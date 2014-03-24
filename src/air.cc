//-------------------------------------------------------------
// file: air.cc
//        (part of gsmsim)
//-------------------------------------------------------------

#include "gsmsim.h"

// The Air object
class Air: public cSimpleModule {
    Module_Class_Members(Air,cSimpleModule,16384)
    virtual void activity();
};

Define_Module(Air);

void Air::activity() {
    int iType, iDest;

    // endless loop
    for (;;) {
        // receive msg
        cMessage *msg = receive();
        iType = msg->kind();
        // send msg to destination
        switch (iType) {
        // MS->BTS
        case CONN_REQ:
        case DISC_REQ:
        case CHECK_BTS:
        case MS_DATA:
        case CHECK_LINE:
            // Transfer these messages from MS to BTS
            iDest = msg->par("dest");
            ev << "Relaying msg to addr=" << iDest << '\n';
            send(msg, "to_bts", iDest);
            break;
            // BTS->MS
        case CONN_ACK:
        case DISC_ACK:
        case FORCE_DISC:
        case HANDOVER_MS:
        case CHECK_MS:
        case BTS_DATA:
            // Transfer these messages from BTS to MS
            iDest = msg->par("dest");
            ev << "Relaying msg to addr=" << iDest << '\n';
            send(msg, "to_ms", iDest);
            break;
        }
    }
}
