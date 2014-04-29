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

package gsm_sim;

import inet.mobility.models.LinearMobility;
import inet.mobility.IMobility;
import inet.linklayer.radio.GenericRadio;
import inet.base.NotificationBoard;
//
// TODO auto-generated module
//
module MS_v2
{
    parameters:
        @node();
        @labels(node);
        @display("i=device/cellphone");
        string imsi = default("00101");
        double initialX = default(0);
        double initialY = default(0);
        string mobilityType = default("LinearMobility");
        @statistic[coordX](title="coord X"; source="xCoord(mobilityPos(mobilityStateChanged))"; record=vector; interpolationmode=linear);
        @statistic[coordY](title="coord Y"; source="yCoord(mobilityPos(mobilityStateChanged))"; record=vector; interpolationmode=linear);
        @statistic[attemptedCall](title="The number of attempted calls"; record=count,vector );
        @statistic[successfulCall](title="The number of successful calls"; record=count,vector );
        @statistic[missedCall](title="The number of missed calls"; record=count,vector );
        @statistic[brokenCall](title="The number of broken calls"; record=count,vector );
        @statistic[handoverCall](title="The number of handover calls"; record=count,vector );
    gates:
        input radioIn @directIn;
    submodules:
        ms_logic: MS {
            parameters:
                imsi = imsi;
                @display("p=71,110");
        }
        mac: GSMMac {
            parameters:
                @display("p=71,175");
        }
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=32,31");
        }
        mobility: <mobilityType> like IMobility {


            @display("p=152,214");
        }
        radio: GSMRadioModule {
            parameters:
                bandwidth = 200000Hz;
                modulation = "BPSK";// This is inaccurate, but we use for now
                headerLengthBits = 8b;
                bitrate = 10000bps;
                @display("p=71,250");
        }

    connections:
        radio.upperLayerIn <-- mac.lowerLayerOut;
        radio.upperLayerOut --> mac.lowerLayerIn;
        mac.upperLayerOut --> ms_logic.from_air;
        mac.upperLayerIn <-- ms_logic.to_air;
        radioIn --> { @display("m=s"); } --> radio.radioIn;
        //from_air --> ms_logic.from_air;
        //to_air <-- ms_logic.to_air;

}
