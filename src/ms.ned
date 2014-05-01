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
     	string imsi;
     	double beaconListenInterval @unit(s) = default(3s); //how long to re-calculate beacon connection
    	double scanDuration @unit(s) = default(1s); //How long to listen for beacons before deciding on the strongest. SHould be longer than the BTS beacon interval.
    	@statistic[RSSI](title="RSSI"; source="receivedRSSI"; record=vector,stats; interpolationmode=none);
    	@statistic[attemptedCall](title="The number of attempted calls"; record=count,vector(count),vector );
        @statistic[successfulCall](title="The number of successful calls"; record=count,vector(count),vector );
        @statistic[missedCall](title="The number of missed calls"; record=count,vector(count),vector );
        @statistic[brokenCall](title="The number of broken calls"; record=count,vector(count),vector );
        @statistic[handoverCall](title="The number of handover calls"; record=count,vector(count),vector );
        @statistic[disconnectedCall](title="The number of disconnected calls"; record=count,vector(count),vector );
        @statistic[failedScan](title="The number of failed scans"; record=count,vector(count),vector );
    gates: 
        input from_air; // gate for connecting to BTS
        output to_air; // gate for connecting to BTS 	
}
