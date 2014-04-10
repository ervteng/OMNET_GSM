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
import inet.linklayer.radio.GenericRadio;
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
    gates:
        input from_air; // gate for connecting to BTS
        output to_air; // gate for connecting to BTS 
        input radioIn @directIn;
    submodules:
        ms_logic: MS{
            parameters:
                @display("p=71,110");
        }        
        mobility: LinearMobility {
            parameters:
                @display("p=90,40");
        }
        radio: GenericRadio{
            parameters:
                bandwidth = 200000Hz;
                modulation = "BPSK";// This is inaccurate, but we use for now
                @display("p=71,170");
        }
        
    connections:
        radio.upperLayerIn <-- ms_logic.to_air;
        radio.upperLayerOut --> ms_logic.from_air;
        radioIn --> { @display("m=s"); } --> radio.radioIn;
        //from_air --> ms_logic.from_air;
        //to_air <-- ms_logic.to_air;
        
}
