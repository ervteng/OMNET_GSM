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

#ifndef __GSM_SIM_GSMMac_H_
#define __GSM_SIM_GSMMac_H_

#include <omnetpp.h>
#include "INETDefs.h"
#include "RadioState.h"
#include "WirelessMacBase.h"
/**
 * TODO - Generated class
 */
class INET_API GSMMac : public WirelessMacBase, public cListener
{
  protected:
    // MacBase functions
    virtual void flushQueue();
    virtual void clearQueue();
    virtual InterfaceEntry *createInterfaceEntry();

    virtual void handleSelfMsg(cMessage *msg);
    virtual void handleUpperMsg(cPacket *msg);
    virtual void handleCommand(cMessage *msg);
    virtual void handleLowerMsg(cPacket *msg);
    cModule *radioModule;
    RadioState::State radioState;
    virtual void receiveSignal(cComponent *src, simsignal_t id, long x);
    static simsignal_t radioStateSignal;
  private:
    simtime_t lastTransmitTime;

  public:
      GSMMac();
      virtual ~GSMMac();

    protected:
      virtual int numInitStages() const { return 2; }
      virtual void initialize(int stage);
};

#endif
