
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <windows.h>
#endif




void usage(int exit_val) {
    printf("The random-hab makes up Nodes and Resources, and reports them to Bionet.\n\
The Resources change over time, and Nodes come and go,\n\
providing a reasonable test load.\n\
\n\
usage:  random-hab [OPTIONS]\n\
\n\
OPTIONS:\n\
\n\
    --help, -h   Show this usage information.\n\
\n\
    --id, -i ID                  Use ID as the HAB-ID (defaults to\n\
                                 hostname if omitted).\n\
\n\
    --min-nodes N                If there are fewer than N nodes, make up\n\
                                 some more.\n\
\n\
    --max-delay X                After taking each action (adding or\n\
                                 removing a Node, or updating a Resource),\n\
                                 the random-hab sleeps up to this long (in\n\
                                 seconds).\n\
\n\
    --output-mode MODE           Available modes are 'normal',\n\
                                 'bdm-client', and 'bionet-watcher'\n\
\n\
");

    exit(exit_val);
}

