
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>

void usage(void) {

    fprintf(stdout, 
	    "'speedway' Hardware Abstractor\n"
	    "\n"
	    "usage: speedway [OPTIONS] TARGET\n"
	    "\n"
	    " -?,-h,--help            Show this help.\n"
	    " -i,--id <ID>            Set the HAB-ID to ID (hostname)\n"
	    " -d,--gpi-delay <milli>  After a GPI1 event, wait N milliseconds before\n"
	    "                         starting to scan for tags (0)\n"
	    " -p,--gpi-polarity <N>   Trigger a tag scan when the value of GPI1 becomes N.\n"
	    "                         Valid values are 0 and 1 (1)\n"
	    " -n,--num-scans <N>      After a GPI trigger, do a sequence of N scans (1)\n"
	    " -l,--scan-idle <N>      Terminate scan early after N milliseconds without no\n"
	    "                         new tag (3000)\n"
	    " -t,--scan-timeout <N>   Stop scanning after N milliseconds (5000)\n"
	    " -c,--periodic-trigger   Enables periodic trigger. Defaults to on.\n"
	    " -u,--null-trigger       Enables null trigger, so it starts when we tell it to.\n"
	    " -g,--gpi-trigger        Enables GPI trigger, so it starts when we push a button.\n"      
	    " -m,--show-messages      Printing all messages between the HAB and the Speedway device\n"
	    " -s,--security-dir <dir> Directory containing security certificates\n"
	    " -v,--version            Print version number\n"
	    "\n"
	    "TARGET is the hostname or IP of a Speedway device.\n");
}

