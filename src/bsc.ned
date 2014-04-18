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
import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.base.NotificationBoard;

//
// Interface declaration for different modules implementing Base Station Controller
//
simple BSC
{
    parameters:
        int phones; // number of phones
        int numbts; // number of bts
    gates:
        input from_bts[]; // gate for connecting to BTS
        output to_bts[]; // gate for connecting to BTS
    
//    submodules:
//        notificationBoard: NotificationBoard {
//            parameters:
//                @display("p=32,30");
//        }
//        mobility: StationaryMobility {
//            parameters:
//                @display("p=200,0,0");
////                initialX = 0m;  // Default position
////                initialY = 0m;
//                initialZ = 0m;   
//        }
}