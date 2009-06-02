
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "test-pattern-hab.h"
#include "hardware-abstractor.h"
#include "bionet.h"



void usage(void) {
    printf("usage: test-pattern-hab [-i ID] Filename\n");
    printf("       test-pattern-hab --help\n");
    printf("\n");
    printf("    Filename is the name of the file containing the config info for the file\n");
    printf("    ID is the desired HAB ID\n");
    printf("\n");
}




int main(int argc, char *argv[]) {
    int opt, bionet_fd;
    char *file_name;
    char *id = NULL;
    //GSList *events = NULL;
    FILE *fd;
    struct timeval *tv;

    while ((opt = getopt(argc, argv, "ih:")) != -1) {
        switch (opt) {
            case 'i':
                id = optarg;
                break;
            case 'h':
            default:
                usage();
        }
    }

    file_name = argv[optind];
    if (file_name == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "need an input file\n");
        usage();
        exit(1);
    }

    //
    //  Create the HAB & connect to bionet
    // 

    hab = bionet_hab_new("test-pattern-hab", id);
    if (hab == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "unable to create new hab: %s", strerror(errno));
    }

    bionet_fd = hab_connect(hab);
    if (bionet_fd < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "hab_connect failed: %s", strerror(errno));
    }

    {
        time_t start, now;
        struct timeval diff;
        fd_set fds;
        int r;

        start = time(NULL);
        now = time(NULL);
        diff.tv_usec = 0;

        do {

            diff.tv_sec = now - start;

            FD_ZERO(&fds);
            FD_SET(bionet_fd, &fds);

            r = select(bionet_fd+1, &fds, NULL, NULL, &diff);

            hab_read();

            now = time(NULL);
        } while (now - start < 2);
    }

    //
    //  parse the input file
    // 

    yyin = fopen(file_name, "r");
    yyrestart(yyin);
    yyparse(fd);

    //
    // dump for each node
    //

    tv = NULL;
    g_slist_foreach(events, simulate_updates, &tv);

    hab_disconnect();

    return 0;
}

