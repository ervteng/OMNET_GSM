package gsm_sim;

simple BTS
{
    parameters:
        int slots; // how many connection can be established
        double watt = default(50); // the watt
        double phones; // number of phones
        string BCC = default("000");
        double beaconInterval @unit(s) = default(1s);
//        @signal[connReqFromMs](type=int);
        @statistic[connReqFromMs](title="Connection Request From MS"; record=stats,vector );
        @statistic[sendBeacon](title="Send beacon"; record=stats,vector );
        @statistic[checkLineFromMs](title="Line check"; source="checkLineFromMs"; record=stats,vector);
    gates:
        input from_air; // gate for connecting to MS
        input from_bsc; // gate for connecting to BSC
        output to_air; // gate for connecting to MS
        output to_bsc; // gate for connecting to BSC
}