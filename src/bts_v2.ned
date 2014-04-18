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

import inet.mobility.models.StationaryMobility;
import inet.linklayer.radio.GenericRadio;
import inet.base.NotificationBoard;
import ned.DatarateChannel;
//
// TODO auto-generated module
//
module BTS_v2
{
    parameters:
        @node();
        @labels(node);
        @display("i=device/antennatower");
        //string imsi = default("00101");
        double initialX = default(0);
        double initialY = default(0);
        double initialZ = default(0);
    gates:
        input from_bsc;
        output to_bsc;
        input radioIn @directIn;


    submodules:
        bts_logic: BTS {
            parameters:
                @display("p=71,110");
        }
        mac: GSMMac {
            parameters:
                @display("p=71,175");
        }
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=32,30");
        }
        mobility: StationaryMobility {
            parameters:
                @display("p=110,38,0");
                //                initialX = 0m;  // Default position
                //                initialY = 0m;
                initialZ = 0m;
        }
        radio: GSMRadioModule {
            parameters:
                bandwidth = 200000Hz;
                modulation = "BPSK";// This is inaccurate, but we use for now
                headerLengthBits = 8b;
                bitrate = 10000bps;
                @display("p=71,250");
        }

    connections :
        radio.upperLayerIn <-- mac.toRadio;
        radio.upperLayerOut --> mac.fromRadio;
        mac.upperLayerOut --> bts_logic.from_air;
        mac.upperLayerIn <-- bts_logic.to_air;
        radioIn --> { @display("m=s"); } --> radio.radioIn;

		// Replace the following with Abis
        from_bsc --> bts_logic.from_bsc;
        to_bsc <-- bts_logic.to_bsc;

}
