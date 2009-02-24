
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>


void print_help(void) {
    printf("usage: csa-cp-hab\n\
\n\
    command line arguments are: \n\
\n\
        -?, --help \n\
          prints this help\n\
\n\
        -i ID, --id ID \n\
          set the HAB-ID to ID, ex. \'-i \"briar-patch\"\'\n\
          defaults to hostname if omitted\n\
\n\
        --no-files\n\
          Do not record the raw CSA-CP data to disk.\n\
\n\
        -d DF, --device-file DF\n\
          talk to relay node through devie file DF\n\
          required\n\
\n\
        -c, --clear-files\n\
          clears the files on the device\n\
          (default is off)\n\
\n\
        --debug\n\
          Enable debugging output.\n\
\n");
}

