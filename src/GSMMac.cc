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

Define_Module(GSMMac);

void GSMMac::initialize()
{
    upperLayerIn = findGate("upperLayerIn");
    upperLayerOut = findGate("upperLayerOut");
    fromRadio = findGate("fromRadio");
    toRadio = findGate("toRadio");
}


void GSMMac::handleMessage(cMessage *msg)
{
      if (msg->getArrivalGateId() == upperLayerIn)
      {
          EV << "Sending Message to Radio\n";
          handleUpperMsg(msg);
      }
      else if (msg->getArrivalGateId() == fromRadio)
      {
          EV << "Sending Message to Logic\n";
          handleRadioMsg(msg);
      }
      else if (msg->isSelfMessage())
      {

      }

}

void GSMMac::handleUpperMsg(cMessage *msg)
{
    cPacket *data = dynamic_cast<cPacket*>(msg);
    data->setByteLength(48);
    send(data,"toRadio");
    //msg = NULL;
}

void GSMMac::handleRadioMsg(cMessage *msg)
{
    send(msg,"upperLayerOut");
}
