
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
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
	    " -?,-h,--help                 Show this help.\n"
	    " -i,--id <ID>                 Set the HAB-ID to ID (hostname)\n"
	    " -d,--gpi-delay <milli>       After a GPI1 event, wait N milliseconds before\n"
	    "                              starting to scan for tags (0)\n"
	    " -p,--gpi-polarity <N>        Trigger a tag scan when the value of GPI1 becomes N.\n"
	    "                              Valid values are 0 and 1 (1)\n"
	    " -n,--num-scans <N>           After a GPI trigger, do a sequence of N scans (1)\n"
	    " -r,--rfsensitivity <N>       Set the RF Sensitivity to index N in dB (0)\n"
	    " -l,--scan-idle <N>           Terminate scan early after N milliseconds without no\n"
	    "                              new tag (3000)\n"
	    " -t,--scan-timeout <N>        Stop scanning after N milliseconds (5000)\n"
	    " -c,--immediate-trigger       Enables immediate trigger. Defaults to on.\n"
	    " -u,--null-trigger            Enables null trigger, so it starts when we tell it to.\n"
	    " -g,--gpi-trigger             Enables GPI trigger, so it starts when we push a button.\n"      
	    " -m,--show-messages           Printing all messages between the HAB and the Speedway device\n"
	    " -S,--simple-report           Enable simple reports\n"
	    " -a,--antenna-id <N>          Select antenna N, default all (0)\n"
	    " -C,--disable-scrub           Disable scrubbing the reader configuration\n"
	    //	    " -x,--tx-power <N>       Transmit Power in dB (30)\n"
	    " -T,--txpower-index <N>       Set the TX Power Index (61)\n"
	    //	    " -D,--tag-direction      Enable tag direction reporting\n"
	    " -A,--antenna-squelch         Enable squelching of the antenna-id boolean resource\n"
	    "                              causing new datapoints only to be published when the state\n"
	    "                              of the resource changes.\n"
	    " -P,--peak-rssi-squelch <N>   Set the Peak RSSI squelch value. Datapoint updates will not be\n"
	    "                              published until a threshold of N is reached.\n"
	    " -f,--peak-rssi-frequency <N> Minimum frequency for Peak RSSI to be pulished (seconds).\n"
	    "                              Causes Peak RSSI datapoint updates to be published if N seconds\n"
	    "                              have expired since last datapoint even if the squench value is\n"
	    "                              not reached.\n"
	    " -s,--security-dir <dir>      Directory containing security certificates\n"
	    " -v,--version                 Print version number\n"
	    "\n"
	    "TARGET is the hostname or IP of a Speedway device.\n");
}

