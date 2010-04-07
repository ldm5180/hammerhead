
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <ctype.h>
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

int loops = -1; 
int simulate_loops = 0; 
int fast = 0;
int current_time = 0;
int no_node_updates = 0;
int max_per_sec = 0;
int s = -1;
struct timeval tv_wait = { 0, 0 };

static char * security_dir = NULL;
static int require_security = 0;

om_t output_mode = OM_NORMAL;

#define MAX_FILE_NAME_LENGTH (200)

void usage() {
    fprintf(stderr, 
	    "'test-pattern-hab' publishes a preset test pattern of Bionet traffic.\n"
	    "\n"
	    "Publishes data from an input file.\n"
	    "\n"
	    "usage:  test-pattern-hab [OPTIONS] INPUT-FILENAME\n"
	    "\n"
	    " -?,--help                     Show this usage information.\n"
	    " -v,--version                  Show the version number\n"
	    " -c,--current-time             Use the current time in the timestamp, file timestamps\n"
	    "                               are used for interval only.\n"
	    " -e,--require-security         Require security\n"
	    " -f,--fast                     Publish as fast as possible, ignoring timestamp intervals.\n"
	    " -i,--id <ID>                  Use ID as the HAB-ID (defaults to\n"
	    "                               hostname if omitted).\n"
	    " -l,--loops <NUM>              Number of times to publish the data consecutively.\n"
            "                               0 means loop forever.\n"
	    " -m,--max-rate <NUM>           Max number of datapoints to publish\n"
	    "                               per second. Only applies to 'fast'\n"     
	    "                               mode.\n"
	    " -n,--no-node-updates          Skip new/lost node messages when looping.\n"
	    " -o,--output-mode <mode>       Available modes are 'normal',\n"
	    "                               'bionet-watcher', 'nodes-only', 'resources-only'\n"
	    " -s,--security-dir <dir>       Directory containing security certificates\n"
	    "\n"
	    "Security can only be required when a security directory has been specified.\n"
	    "  test-pattern-hab [--security-dir <dir> [--require-security]]\n");
} /* usage() */


int main(int argc, char *argv[]) {
    int bionet_fd, i;
    char file_name[MAX_FILE_NAME_LENGTH];
    char *id = NULL;
    //GSList *events = NULL;
    struct timeval *tv;

    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };
    g_log_set_default_handler(bionet_glib_log_handler, &log_context);

    while (1) {
        int c;
        static struct option long_options[] = {
            {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"current-time", 0, 0, 'c'},
	    {"require-security", 0, 0, 'e'},
	    {"fast", 0, 0, 'f'},
            {"id", 1, 0, 'i'},
	    {"loops", 1, 0, 'l'},
	    {"max-rate", 1, 0, 'm'},
	    {"no-node-updates", 0, 0, 'n'},
            {"output-mode", 1, 0, 'o'},
	    {"security-dir", 1, 0, 's'},
            {0, 0, 0, 0} //this must be last in the list
        };

        c = getopt_long(argc, argv, "?vcefns:i:o:l:m:", long_options, &i);
        if (c == -1)
            break;

        switch (c) {

	case '?':
	    usage();
	    exit(0);

	case 'v':
	    print_bionet_version(stdout);
	    exit(0);

	case 'c':
	    current_time++;
	    break;

	case 'e':
	    require_security++;
	    break;

	case 'f':
	    fast++;
	    break;

	case 'i':
	    id = optarg;
	    break;

	case 'l':
	    loops = atoi(optarg);
	    break;

	case 'm':
	    max_per_sec = atoi(optarg);
	    break;

	case 'n':
	    no_node_updates++;
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

	case 's':
	    security_dir = optarg;
	    break;

	default:
	    break;
        }
    }

    if ((require_security) && (security_dir == NULL)) {
	g_log("", G_LOG_LEVEL_ERROR, "Security directory is required when security is required.");
	usage();
	exit(1);
    }

    if (max_per_sec && (0 == fast)) {
	g_log("", G_LOG_LEVEL_ERROR, "--max-per-sec only applies to 'fast' mode.");
	usage();
	exit(1);
    } else if (max_per_sec) {
	tv_wait.tv_sec = 0;
	tv_wait.tv_usec = 1000000L/max_per_sec;
    }

    if (optind == argc-1) {
        strncpy(file_name, argv[optind], MAX_FILE_NAME_LENGTH);
        if (MAX_FILE_NAME_LENGTH > 0) {
            file_name[MAX_FILE_NAME_LENGTH - 1] = '\0';
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
    
    hab_register_callback_set_resource(NULL);

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

        while (now - start < 2) {
            diff.tv_sec = 2 - (now - start);

            FD_ZERO(&fds);
            FD_SET(bionet_fd, &fds);

            r = select(bionet_fd+1, &fds, NULL, NULL, &diff);

            hab_read();

            now = time(NULL);
        }
    }

    //
    //  parse the input file
    // 

    yyin = fopen(file_name, "r");
    if (yyin == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "unable to open file '%s': %s", file_name, strerror(errno));
    }
    yyrestart(yyin);
    yyparse();

    if (output_mode == OM_BIONET_WATCHER)
        g_message("new hab: %s", bionet_hab_get_name(hab));

    //
    // dump for each node
    //

    simulate_loops = 0;
    do {	
	tv = NULL;
	g_slist_foreach(events, simulate_updates, &tv);
    } while (
        (loops == 0)
        || ((loops > 0) && (++simulate_loops < loops))
    );
    
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

