
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "test-pattern-hab.h"
#include "hardware-abstractor.h"
#include "bionet-util.h"


om_t output_mode = OM_NORMAL;


void usage(void) {
    printf("usage: test-pattern-hab [-i ID] [-o OUTPUT_MODE] FILENAME\n");
    printf("       test-pattern-hab --help\n");
    printf("\n");
    printf("    FILENAME is the name of the file containing the config info for the file\n");
    printf("    ID is the desired HAB ID\n");
    printf("    OUTPUT_MODE is either \"normal\" (default) or \"bionet-watcher\"\n");
    printf("                or \"nodes-only\" or \"resources-only\"\n");
    printf("\n");
}




int main(int argc, char *argv[]) {
    int bionet_fd, i;
    char *file_name;
    char *id = NULL;
    //GSList *events = NULL;
    FILE *fd;
    struct timeval *tv;

    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };
    g_log_set_default_handler(bionet_glib_log_handler, &log_context);

    while (1) {
        int c;
        static struct option long_options[] = {
            {"help", 0, 0, 'h'},
            {"id", 1, 0, 'i'},
            {"output-mode", 1, 0, 'o'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "?hi:o:", long_options, &i);
        if (c == -1)
            break;

        switch (c) {
            case 'i':
                id = optarg;
                break;
            case 'o': {
                if (strcmp(optarg, "normal") == 0) 
                    output_mode = OM_NORMAL;
                else if (strcmp(optarg, "bionet-watcher") == 0) 
                    output_mode = OM_BIONET_WATCHER;
                else if (strcmp(optarg, "nodes-only") == 0) 
                    output_mode = OM_NODES_ONLY;
                else if (strcmp(optarg, "resources-only") == 0) 
                    output_mode = OM_RESOURCES_ONLY;
                else {
                    g_log("", G_LOG_LEVEL_WARNING, "unknown output mode %s", optarg);
                    usage();
                }
                break;
            }
            case '?':
            case 'h':
                usage();
                break;
            default:
                break;
        }
    }

    if (optind == argc-1) {
        file_name = argv[optind];
        if (file_name == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "need an input file");
            usage();
            exit(1);
        }
    } else if (optind < argc-1 ) {
        g_log("", G_LOG_LEVEL_WARNING, "too many input files");
        usage();
        exit(1);
    } else {
        g_log("", G_LOG_LEVEL_WARNING, "need at least a single input file");
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
    if (yyin == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "unable to open file '%s': %s", file_name, strerror(errno));
    }
    yyrestart(yyin);
    yyparse(fd);

    if (output_mode == OM_BIONET_WATCHER)
        g_message("new hab: %s", bionet_hab_get_name(hab));

    //
    // dump for each node
    //

    tv = NULL;
    g_slist_foreach(events, simulate_updates, &tv);

    if (output_mode == OM_BIONET_WATCHER)
        g_message("lost hab: %s", bionet_hab_get_name(hab));

    hab_disconnect();
    bionet_hab_free(hab);

    while (events != NULL) {
        free_event((struct event_t*)events->data);
        events->data = NULL;
        events = g_slist_delete_link(events, events);
    }

    return 0;
}

