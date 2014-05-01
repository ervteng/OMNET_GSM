package gsm_sim;
//-------------------------------------------------------------
// file: gsmsim.ned
//        (part of GSMSIM - an OMNeT++ simulation)
//
//-------------------------------------------------------------


//import inet.examples.adhoc.hostautoconf.Host;
import inet.world.radio.ChannelControl;
//import ned.DatarateChannel;

//
// Declaration of simple module type Air (interface)
//
//simple Air
//{
//    gates:
//        input from_ms[]; // input and output gates
//        input from_bts[]; // for communication between MS and BTS
//        output to_ms[];
//        output to_bts[];
//}


//
// Interface declaration for different modules implementing Mobile Station
//


//
// The network
//
network GSMSIM
{
    parameters:
        int number_ms = default(1);
        int number_bts = default(1);
        double xwidth = default(10000);
        double ydepth = default(10000);
    //    types:
    //        channel Abis extends DatarateChannel
    //        {
    //            delay = 10ms;
    //            datarate = 5Mbps;
    //        }
    submodules:
        bsc: BSC {
            parameters:
                numbts = number_bts;
                phones = number_ms;
                @display("p=322,82;i=router");
            gates:
                from_bts[number_bts];
                to_bts[number_bts];
        }
        ms[number_ms]: MS_v2 {
            parameters:
                //xc = input(0,"X position"); // start x position [m]
                //yc = input(0,"Y position"); // start y position [m]	
                //vx = input(5,"Velocity X"); // velocity x [m/s]
                //vy = input(5,"Velocity Y"); // velocity y [m/s]
                ms_logic.vx = default(5); // velocity x [m/s]
                ms_logic.vy = default(5); // velocity y [m/s]
                ms_logic.numbts = number_bts;
                ms_logic.timeout = 1s;
                //@display("p=30,240;r=,,#707070");
                @display("p=262,201");
        }
        bts[number_bts]: BTS_v2 {
            parameters:
                bts_logic.phones = number_ms;
                bts_logic.slots = default(3); // how many users it can hold
                @display("p=30,120");
        }
        malicious_bts: BTS_v2 {
            parameters:
                bts_logic.phones = number_ms;
                bts_logic.slots = default(3); // how many users it can hold
                @display("p=200,246");
        }
        malicious_bsc: BSC {
            parameters:
                numbts = 1;
                phones = number_ms;
                @display("p=301,246;i=router");
            gates:
                from_bts[1];
                to_bts[1];
        }
        channelControl: ChannelControl {
            coreDebug = true;
            @display("p=60,50");
        }

    connections: // make all the connection between the objects
        //endfor; 
        for i=0..number_bts-1 {
            bts[i].to_bsc --> bsc.from_bts[i];
            bts[i].from_bsc <-- bsc.to_bts[i];
        }
        malicious_bts.to_bsc --> malicious_bsc.from_bts[0];
        malicious_bts.from_bsc <-- malicious_bsc.to_bts[0];
        //endfor; 
}

