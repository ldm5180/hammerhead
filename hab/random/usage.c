
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <windows.h>
#endif




void usage() {
    fprintf(stderr, 
	    "'random-hab' publishes random Bionet traffic.\n"
	    "\n"
	    "Makes up Nodes and Resources, and reports them to Bionet.\n"
	    "The Resources change over time, and Nodes come and go,\n"
	    "providing a reasonable test load.\n"
	    "\n"
	    "usage:  random-hab [OPTIONS]\n"
	    "\n"
	    " -h,--help                     Show this usage information.\n"
	    " -v,--version                  Show the version number\n"
	    " -i,--id <ID>                  Use ID as the HAB-ID (defaults to\n"
	    "                               hostname if omitted).\n"
	    " -m,--min-nodes <N>            If there are fewer than N nodes, make up\n"
	    "                               some more.\n"
	    " -x,--max-delay <X>            After taking each action (adding or\n"
	    "                               removing a Node, or updating a Resource),\n"
	    "                               the random-hab sleeps up to this long (in\n"
	    "                               seconds).\n"
	    " -o,--output-mode <mode>       Available modes are 'normal',\n"
	    "                               'bdm-client', and 'bionet-watcher'\n");
}

