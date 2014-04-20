//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "GSMMac.h"

#include "GSMRadio.h"
#include "IdealWirelessFrame_m.h"
#include "Ieee802Ctrl_m.h"
#include "IInterfaceTable.h"
#include "InterfaceTableAccess.h"
#include "IPassiveQueue.h"
#include "opp_utils.h"

Define_Module(GSMMac);

// Register radio State Signal
simsignal_t GSMMac::radioStateSignal = registerSignal("radioState");




GSMMac::GSMMac()
{
    //queueModule = NULL;
    radioModule = NULL;
}

GSMMac::~GSMMac()
{
}

void GSMMac::clearQueue()
{
}

void GSMMac::flushQueue()
{
}

InterfaceEntry *GSMMac::createInterfaceEntry()
{
    InterfaceEntry *e = new InterfaceEntry(this);
    return e;
}

void GSMMac::initialize(int stage)

{
    WirelessMacBase::initialize(stage);
    if(stage == 0)
    {
        //upperLayerIn = findGate("upperLayerIn");
        //upperLayerOut = findGate("upperLayerOut");
        //fromRadio = findGate("fromRadio");
        //toRadio = findGate("toRadio");
        radioModule = gate("lowerLayerOut")->getPathEndGate()->getOwnerModule();
        GSMRadio *irm = check_and_cast<GSMRadio *>(radioModule);

        EV << "Subscribing Listener\n";
        irm->subscribe(radioStateSignal, this);

        // Allow for transmits at time 0
        lastTransmitTime = simTime() - 0.000001;
    }
}


// Set radio State when state is updated by Radio module
void GSMMac::receiveSignal(cComponent *src, simsignal_t id, long x)
{
    if (id == radioStateSignal && src == radioModule)
    {
        radioState = (RadioState::State)x;
    }
}


//void GSMMac::handleMessage(cMessage *msg)
//{
//      if (msg->getArrivalGateId() == upperLayerIn)
//      {
//          EV << "Sending Message to Radio\n";
//          handleUpperMsg(msg);
//      }
//      else if (msg->getArrivalGateId() == fromRadio)
//      {
//          EV << "Sending Message to Logic\n";
//          handleRadioMsg(msg);
//      }
//      else if (msg->isSelfMessage())
//      {
//
//      }
//
//}

//Only send when not transmitting
void GSMMac::handleUpperMsg(cPacket *msg)
{
        //outStandingRequests--;
        if (radioState == RadioState::TRANSMIT)
        {
            // Logic error: we do not request packet from the external queue when radio is transmitting
            //error("Received msg for transmission but transmitter is busy");
        }
        else if (radioState == RadioState::SLEEP)
        {
            EV << "Dropped upper layer message " << msg << " because radio is SLEEPing.\n";
        }
        else
        {
            // We are idle, so we can start transmitting right away.
            EV << "Received " << msg << " for transmission\n";
            cPacket *data = dynamic_cast<cPacket*>(msg);

            //Collision Protection
            if(lastTransmitTime < simTime()){
                sendDown(data);
                lastTransmitTime = simTime();
            }
            else {
                EV << "The last transmission was at the same time, it's collision.\n";
            }

        }


}

void GSMMac::handleSelfMsg(cMessage *msg)
{
    //error("Unexpected self-message");
}

void GSMMac::handleLowerMsg(cPacket *msg)
{
    sendUp(msg);
}

void GSMMac::handleCommand(cMessage *msg)
{
    error("Unexpected command received from higher layer");
}
