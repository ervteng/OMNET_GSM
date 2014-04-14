package gsm_sim;

simple BTS
{
    parameters:
        double xc; // X coordinate
        double yc; // Y coordinate
        int slots; // how many connection can be established
        double watt = default(50); // the watt
        double phones; // number of phones
    gates:
        input from_air; // gate for connecting to MS
        input from_bsc; // gate for connecting to BSC
        output to_air; // gate for connecting to MS
        output to_bsc; // gate for connecting to BSC
}