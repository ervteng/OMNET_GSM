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

#include <GSMRadio.h>

#define MIN_DISTANCE 0.001 // minimum distance 1 millimeter
#define BASE_NOISE_LEVEL (noiseGenerator?noiseLevel+noiseGenerator->noiseLevel():noiseLevel)

GSMRadio::GSMRadio() {
    // TODO Auto-generated constructor stub
    //Radio();
}

GSMRadio::~GSMRadio() {
    //~Radio();
    // TODO Auto-generated destructor stub
}


//void GSMRadio::handleLowerMsgStart(AirFrame* airframe)
//{
//    // Calculate the receive power of the message
//
//    // calculate distance
//    const Coord& framePos = airframe->getSenderPos();
//    double distance = getRadioPosition().distance(framePos);
//
//    // calculate receive power
//    double frequency = carrierFrequency;
//    if (airframe && airframe->getCarrierFrequency()>0.0)
//        frequency = airframe->getCarrierFrequency();
//
//    if (distance<MIN_DISTANCE)
//        distance = MIN_DISTANCE;
//
//    double rcvdPower = receptionModel->calculateReceivedPower(airframe->getPSend(), frequency, distance);
//    RSSI = rcvdPower;
//    if (obstacles && distance > MIN_DISTANCE)
//        rcvdPower = obstacles->calculateReceivedPower(rcvdPower, carrierFrequency, framePos, 0, getRadioPosition(), 0);
//    airframe->setPowRec(rcvdPower);
//    // store the receive power in the recvBuff
//    recvBuff[airframe] = rcvdPower;
//    updateSensitivity(airframe->getBitrate());
//
//    // if receive power is bigger than sensitivity and if not sending
//    // and currently not receiving another message and the message has
//    // arrived in time
//    // NOTE: a message may have arrival time in the past here when we are
//    // processing ongoing transmissions during a channel change
//    if (airframe->getArrivalTime() == simTime() && rcvdPower >= sensitivity && rs.getState() != RadioState::TRANSMIT && snrInfo.ptr == NULL)
//    {
//        EV << "receiving frame " << airframe->getName() << endl;
//
//        // Put frame and related SnrList in receive buffer
//        SnrList snrList;
//        snrInfo.ptr = airframe;
//        snrInfo.rcvdPower = rcvdPower;
//        snrInfo.sList = snrList;
//
//        // add initial snr value
//        addNewSnr();
//
//        if (rs.getState() != RadioState::RECV)
//        {
//            // publish new RadioState
//            EV << "publish new RadioState:RECV\n";
//            setRadioState(RadioState::RECV);
//        }
//    }
//    // receive power is too low or another message is being sent or received
//    else
//    {
//        EV << "frame " << airframe->getName() << " is just noise\n";
//        //add receive power to the noise level
//        noiseLevel += rcvdPower;
//
//        // if a message is being received add a new snr value
//        if (snrInfo.ptr != NULL)
//        {
//            // update snr info for currently being received message
//            EV << "adding new snr value to snr list of message being received\n";
//            addNewSnr();
//        }
//
//        // update the RadioState if the noiseLevel exceeded the threshold
//        // and the radio is currently not in receive or in send mode
//        if (BASE_NOISE_LEVEL >= receptionThreshold && rs.getState() == RadioState::IDLE)
//        {
//            EV << "setting radio state to RECV\n";
//            setRadioState(RadioState::RECV);
//        }
//    }
//}

double GSMRadio::getRSSI() {
    return 3456;
}

double GSMRadio::getSNIR() {
    return snrInfo.sList.begin()->snr;
}
