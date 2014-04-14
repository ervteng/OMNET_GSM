package gsm_sim;
//-------------------------------------------------------------
// file: gsmsim.ned
//        (part of GSMSIM - an OMNeT++ simulation)
//
//-------------------------------------------------------------


import inet.examples.adhoc.hostautoconf.Host;
import inet.world.radio.ChannelControl;


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
// Interface declaration for different modules implementing Base Transciever Station
//



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
}


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
    submodules:
//        air: Air {
//            parameters:
//                @display("p=120,180;i=cloud");
//            gates:
//                to_ms[number_ms];
//                from_ms[number_ms];
//                to_bts[number_bts];
//                from_bts[number_bts];
//        }
            //parameters:
           		//@display("p=120,180;i=cloud");
        //}
        bsc: BSC {
            parameters:
                numbts = number_bts;
                phones = number_ms;
                @display("p=120,50;i=router");
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
                ms_logic.xc = default(0); // start x position [m]
                ms_logic.yc = default(0); // start y position [m]	
                ms_logic.vx = default(5); // velocity x [m/s]
                ms_logic.vy = default(5); // velocity y [m/s]
                ms_logic.numbts = number_bts;
                ms_logic.timeout = 1s;
                //@display("p=30,240;r=,,#707070");
        }
        bts[number_bts]: BTS_v2 {
            parameters:
                bts_logic.phones = number_ms;
                //xc = input(10000,"X coordinate"), // X coordinate
                // yc = input(10000,"Y coordinate"), // Y coordinate
                bts_logic.xc = default(10000); // X coordinate
                bts_logic.yc = default(10000); // Y coordinate
                //slots = input(3,"Max. users"); // how many users it can hold
                bts_logic.slots = default(3); // how many users it can hold
                @display("p=30,120");
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
        //endfor; 
}


//network gsmsim : GSMSIM {
//    parameters: 
////        number_bts = input(1,"Number of stations:"); 
////        number_ms = input(1,"Number of cars:"); 
////        xwidth = input(10000,"Width of the plane [m]:"); 
////        ydepth = input(10000,"Depth of the plane [m]:"); 
//        number_bts = 1; 
//        number_ms = 1; 
//        xwidth = 10000; 
//        ydepth = 10000; 
//} 

