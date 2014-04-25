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
//
// TODO auto-generated module
//
simple MS 
{
    parameters: 
      	int numbts; // how many bts
     	double timeout @unit(s) = default(1s); // how long do you wait before timeout 
     	int imsi;
    gates: 
        input from_air; // gate for connecting to BTS
        output to_air; // gate for connecting to BTS 	
}
