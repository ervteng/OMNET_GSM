package gsm_sim;

simple BTS
{
    parameters:
        int slots; // how many connection can be established
        double watt @unit(W) = default(50W); // the watt
        double phones; // number of phones
        string BCC = default("000");
        double beaconInterval @unit(s) = default(1s);
        double turnOnTime @unit(s) = default(0.1s); //When to turn on the BTS 
//        @signal[connReqFromMs](type=int);
        @statistic[connReqFromMs](title="Connection Request From MS"; record=count );
        @statistic[sendBeacon](title="Send beacon"; record=count );
        @statistic[numConnections](title="The number of connections"; record=count,vector );
        @statistic[errNoSlot](title="Error Not Slot"; record=count,vector );
        @statistic[handOverToBsc](title="Count handover to BSC"; record=count,vector );
        @statistic[handOverToMs](title="Count handover to MS"; record=count,vector );
        @statistic[handOverCheckToBsc](title="Count handover check to BSC"; record=count,vector );
        @statistic[handOverCheckFromMs](title="Count handover check from MS"; record=count,vector );
        @statistic[discRequest](title="Disconnect Request from MS"; record=count,vector );
        @statistic[forceDisc](title="Force Disconnect to MS"; record=count );
        

//        @statistic[checkLineFromMs](title="Line check"; source="checkLineFromMs"; record=stats,vector);
        
//        // Dynamic stats
////        @signal[phone*-rssi](type=double); // note the wildcard
//        @statisticTemplate[phoneRssi](record=stats,vector);
        
    gates:
        input from_air; // gate for connecting to MS
        input from_bsc; // gate for connecting to BSC
        output to_air; // gate for connecting to MS
        output to_bsc; // gate for connecting to BSC
}