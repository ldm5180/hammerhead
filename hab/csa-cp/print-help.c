
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>


void print_help(void) {
    fprintf(stdout, 
	    "'csa-cp-hab' exports data from CSA-CP device to Bionet\n"
	    "\n"
	    "usage: csa-cp-hab [OPTIONS] --device-file <DF>\n"
	    "\n"
	    "-?,-h,--help              Print this help\n"
	    "-b,--debug	               Enable debugging output.\n"
	    "-c,--clear-files          Clears the files on the device\n"
	    "-d,--device-file <DF>     Talk to relay node through device file DF (required)\n"
	    "-i,--id <ID>              Set the HAB-ID to ID (default: hostname)\n"
	    "-n,--no-files             Do not record the raw CSA-CP data to disk.\n"
	    "-s,--security-dir <dir>   Directory containing security certificates\n"
	    "-v,--version              Print version number\n");
}

