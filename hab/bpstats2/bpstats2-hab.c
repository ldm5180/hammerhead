
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

// Receives BP bundles from bpstats2, and publishes the info as a bionet hab.

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <glib.h>

#include "bpstats2-hab.h"

/* Mutex: Only one thread can do Bionet stuff at a time. */
pthread_mutex_t  doing_bionet_mutex;

/* When threads find this variable is 0, they cleanup and return. */
int keep_running = 1;
void signal_handler(int unused) {
    keep_running = 0;

    /* Interrupt bp_receive() */
    bp_interrupt(sap);
}

//
// Parse command-line options
//
void handle_options(int argc, char *argv[])
{
    int i;

    while(1) {
    int c;
    static struct option long_options[] = {
        {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"id", 1, 0, 'i'},
	    {"security-dir", 1, 0, 's'},
	    {"require-security", 0, 0, 'e'},
        {"smkey", 1, 0, 'k'},
	    {0, 0, 0, 0} //this must be last in the list
	};

    c = getopt_long(argc, argv, "?hvei:s:E:k:", long_options, &i);
    if(c == -1) break;

    switch (c) {
    case '?':
    case 'h':
        usage();
        exit(0);

    case 'e':
        if (security_dir) {
            require_security++;
        } else {
            usage();
            exit(1);
        }
        break;

    case 'i':
        hab_id = optarg;
        break;

    case 's':
        security_dir = optarg;
        break;

    case 'v':
        print_bionet_version(stdout);
        exit(0);
        break;

    case 'k':
#ifdef ENBABLE_ION_BIOSERVE
        sm_set_basekey(atoi(optarg));
#else
        fprintf(stderr, "Bioserve version of ION required to set Base Key.\n");
#endif
        break;
    }
    } //while(1)

    /* Get our EID */
    if(optind >= argc) {
        fprintf(stderr, "No EID specified.\n");
        usage();
        exit(1);
    }
    eid = argv[optind];
    optind++;
}

//
// Initialize a hab
//
void bionet_init()
{
    hab = bionet_hab_new(hab_type, hab_id);

    if(security_dir 
            && hab_init_security(security_dir, require_security)) {
        g_warning("Failed to initialize security.");
    }

    bionet_fd = hab_connect(hab);
    if(bionet_fd < 0) {
        g_error("Couldn't connect to Bionet.");
    }
}

//
// Initialize an ION BP endpoint ID
//
void ion_init()
{
    if (bp_attach() < 0) {
        g_error("Can't attach to BP.");
    }

    if (bp_open(eid, &sap) < 0) {
        g_error("Can't open endpoint %s.", eid);
    }

    sdr = bp_get_sdr();
}


int main(int argc, char *argv[])
{
    handle_options(argc, argv);

    bionet_init();
    ion_init();

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    if(pthread_mutex_init(&doing_bionet_mutex, NULL) != 0)
    {
        g_error("Couldn't init mutex: %s", strerror(errno));
    }

    if(pthread_create(&handle_hab_read_thread, NULL,
            handle_hab_read, NULL) < 0) {
        g_error("Can't make handle_hab_read_thread: %s", strerror(errno));
    }

    if(pthread_create(&handle_bundles_thread, NULL,
            handle_bundles, NULL) < 0) {
        g_error("Can't make handle_bundles_thread: %s", strerror(errno));
    }

    pthread_join(handle_bundles_thread, NULL);
    /* We don't join on handle_hab_read_thread: if handle_bundles_thread 
     * returns, we'll never publish new data so we should die rather than
     * maintain stale data. */

    bp_close(sap);

    g_debug("bpstats2 exiting normally");
    return 0;
}
