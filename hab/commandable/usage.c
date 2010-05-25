
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>




void usage(void) {
    fprintf(stderr, 
	    "'commandable-hab' publishes several Resources that can be commanded.\n"
	    "\n"
	    "usage:  commandable-hab [OPTIONS]\n"
	    "\n"
	    " -?,--help                     Show this usage information.\n"
	    " -v,--version                  Show the version number\n"
	    " -e,--require-security         Require security\n"
	    " -i,--id <ID>                  Use ID as the HAB-ID (defaults to\n"
	    "                               hostname if omitted).\n"
	    " -s,--security-dir <dir>       Directory containing security certificates\n"
	    "\n"
	    "Security can only be required when a security directory has been specified.\n"
	    "  commandable-hab [--security-dir <dir> [--require-security]]\n");
}

