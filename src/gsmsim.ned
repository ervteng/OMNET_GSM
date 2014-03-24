package gsm_sim;
//-------------------------------------------------------------
// file: gsmsim.ned
//        (part of GSMSIM - an OMNeT++ simulation)
//
//-------------------------------------------------------------


//
// Declaration of simple module type Air (interface)
//
simple Air 
{
    gates: 
        input from_ms[]; // input and output gates
        input from_bts[]; // for communication between MS and BTS
        output to_ms[]; 
        output to_bts[]; 
} 


//
// Interface declaration for different modules implementing Mobile Station
//
simple MS 
{
    parameters: 
        double xc; // X coordinate
        double yc; // Y coordinate
       	double vx; // velocity X vector
        double vy; // velocity Y vector
      	int numbts; // how many bts
     	double timeout @unit(s); // how long do you wait before timeout 
    gates: 
        input from_air; // gate for connecting to BTS
        output to_air; // gate for connecting to BTS
}
 


//
// Interface declaration for different modules implementing Base Transciever Station
//
simple BTS {
    parameters: 
      	double xc; // X coordinate
        double yc; // Y coordinate
        int slots; // how many connection can be established
        double watt; // the watt
        double phones; // number of phones
    gates: 
        input from_air; // gate for connecting to MS
        input from_bsc; // gate for connecting to BSC
        output to_air; // gate for connecting to MS
        output to_bsc; // gate for connecting to BSC
}


//
// Interface declaration for different modules implementing Base Station Controller
//
simple BSC {
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
module GSMSIM {
    parameters: 
        number_ms = default(4); 
        number_bts = default(2); 
        xwidth; 
        ydepth; 
    submodules: 
        air: Air {
            gatesizes: 
                to_ms[number_ms], 
                from_ms[number_ms], 
                to_bts[number_bts], 
                from_bts[number_bts]; 
            display: "p=120,180;i=cloud";
        }
        bsc: BSC{
            parameters: 
                numbts = number_bts, 
                phones = number_ms; 
            gatesizes: 
                from_bts[number_bts], 
                to_bts[number_bts]; 
            display: "p=120,50;i=router";
        }
        ms: MS[number_ms]{
            parameters: 
                xc = input(0,"X position"); // start x position [m]
                yc = input(0,"Y position"); // start y position [m]	
                vx = input(5,"Velocity X"); // velocity x [m/s]
                vy = input(5,"Velocity Y"); // velocity y [m/s]
                numbts = number_bts; 
                timeout = 1s; 
            display: "p=30,240;i=bwgen";
        }
        bts: BTS[number_bts]{
            parameters: 
                phones = number_ms, 
                xc = input(10000,"X coordinate"), // X coordinate
                yc = input(10000,"Y coordinate"), // Y coordinate
                slots = input(3,"Max. users"); // how many users it can hold
            display: "p=30,120;i=ipc";
        }

    connections: // make all the connection between the objects
        for i=0..number_ms-1 do 
            ms[i].to_air --> air.from_ms[i]; 
            ms[i].from_air <-- air.to_ms[i]; 
        endfor; 
        for i=0..number_bts-1 do 
            air.to_bts[i] --> bts[i].from_air; 
            air.from_bts[i] <-- bts[i].to_air; 
            bts[i].to_bsc --> bsc.from_bts[i]; 
            bts[i].from_bsc <-- bsc.to_bts[i]; 
        endfor; 
} 


network gsmsim : GSMSIM {
    parameters: 
        number_bts = input(1,"Number of stations:"), 
        number_ms = input(1,"Number of cars:"), 
        xwidth = input(10000,"Width of the plane [m]:"), 
        ydepth = input(10000,"Depth of the plane [m]:"); 
} 

