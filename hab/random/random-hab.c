
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#ifdef WINDOWS
    #include <windows.h>
#endif

#include <glib.h>

#include "hardware-abstractor.h"

#include "random-hab.h"




// it sets this true in the signal handler and checks it at the top of the
// main loop, so the hab's output can be consistent with the clients'
int should_exit = 0;

om_t output_mode = OM_NORMAL;
bionet_hab_t *hab;

int urandom_fd;


void show_stuff_going_away(void) {
    int i;

    if (output_mode != OM_BIONET_WATCHER) return;

    for (i = 0; i < bionet_hab_get_num_nodes(hab); i ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, i);
        g_message("lost node: %s", bionet_node_get_name(node));
    }
    g_message("lost hab: %s", bionet_hab_get_name(hab));
}


void signal_handler(int unused) {
    should_exit = 1;
}




void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value) {
    char *value_str = bionet_value_to_str(value);
    if (output_mode == OM_NORMAL) {
        printf(
            "callback: should set %s to '%s'\n",
            bionet_resource_get_local_name(resource),
            value_str
        );
    }
    free(value_str);
}




int main (int argc, char *argv[]) {
    int bionet_fd;
    int i;

    int max_delay = 5;
    int min_nodes = 5;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;

    char * security_dir = NULL;
    int require_security = 0;

    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };

    g_log_set_default_handler(bionet_glib_log_handler, &log_context);

    while(1) {
	int c;
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"id", 1, 0, 'i'},
	    {"min-nodes", 1, 0, 'm'},
	    {"max-delay", 1, 0, 'x'},
	    {"output-mode", 1, 0, 'o'},
	    {"security-dir", 1, 0, 's'},
	    {"require-security", 0, 0, 'e'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?hvei:m:x:o:s:", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {

	case '?':
	case 'h':
	    usage();
	    return 0;

	case 'e':
	    if (security_dir) {
		require_security++;
	    } else {
		usage();
		return (-1);
	    }
	    break;

	case 'i':
	    hab_id = optarg;
	    break;

	case 'm':
	    min_nodes = atoi(optarg);
	    break;

	case 'o':
            if (strcmp(optarg, "normal") == 0) output_mode = OM_NORMAL;
            else if (strcmp(optarg, "bdm-client") == 0) output_mode = OM_BDM_CLIENT;
            else if (strcmp(optarg, "bionet-watcher") == 0) output_mode = OM_BIONET_WATCHER;
            else {
                fprintf(stderr, "unknown output mode %s\n", optarg);
                usage();
		return 1;
            }
	    break;

	case 's':
	    security_dir = optarg;
	    break;

	case 'v':
	    print_bionet_version(stdout);
	    return 0;

	case 'x':
	    max_delay = atoi(optarg);
	    break;

	default:
	    break;
	}
    } //while(1)


    //  
    //  Initializing the hab
    //
    
    hab = bionet_hab_new(hab_type, hab_id);


    if (security_dir) {
	if (hab_init_security(security_dir, require_security)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
	} 
    }

    //
    //  Connecting to Bionet 
    //

    hab_register_callback_set_resource(cb_set_resource);

    bionet_fd = hab_connect(hab);
    if (bionet_fd < 0) {
        fprintf(stderr, "problem connecting to Bionet, exiting\n");
        return 1;
    }

    if (output_mode == OM_BIONET_WATCHER) {
        g_message("new hab: %s", bionet_hab_get_name(hab));
	if (security_dir) {
	    g_message("    %s: security enabled", bionet_hab_get_name(hab));
	}
    }

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);


    //
    // give clients a couple of seconds to connect
    // FIXME: racy hack
    //

    {
        time_t start;

        start = time(NULL);

        do {
            time_t now;
            struct timeval timeout;
            fd_set readers;
            int r;

            now = time(NULL);

            if ((now - start) > 2) break;

            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            FD_ZERO(&readers);
            FD_SET(bionet_fd, &readers);

            r = select(bionet_fd + 1, &readers, NULL, NULL, &timeout);
            if (r < 0) {
                fprintf(stderr, "error from select: %s\n", strerror(errno));
                g_usleep(1000*1000);
                continue;
            }

            hab_read();
        } while(1);
    }

    urandom_fd = open ("/dev/urandom", O_RDONLY);
    if (0 > urandom_fd) {
	g_message("Failed to open /dev/urandom: %m");
	exit(1);
    }

    // 
    // main loop
    // 
    while (1) {
        fd_set readers;
        struct timeval timeout;
        int rnd;
        uint32_t ms_delay;
        int r;

        if (should_exit) {
            hab_disconnect();  // let the CAL thread emit any last-second events we sent *last* time through the main loop
            show_stuff_going_away();
            exit(0);
        }

	hab_read();

        while (bionet_hab_get_num_nodes(hab) < min_nodes) {
            add_node(hab);
        }

        while (bionet_hab_get_num_nodes(hab) > (2 * min_nodes)) {
            destroy_node(hab);
        }

	if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	    continue;
	}
	else
	{
	    rnd = rnd % 100;
	}
        if (rnd < 10) {
            destroy_node(hab);
        } else if (rnd < 20) {
            add_node(hab);
        } else {
            update_node(hab);
        }

        FD_ZERO(&readers);
        FD_SET(bionet_fd, &readers);

	if (sizeof(ms_delay) != read(urandom_fd, &ms_delay, sizeof(ms_delay))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	    continue;
	} else {
	    ms_delay = abs(ms_delay) % (max_delay * 1000);
	}
        timeout.tv_sec = ms_delay / 1000;
        timeout.tv_usec = (ms_delay % 1000) * 1000;

        r = select(bionet_fd + 1, &readers, NULL, NULL, &timeout);
        if (
            (r == 0)
            || ((r < 0) && (errno == EINTR))
        ) {
            // timeout or signal
            continue;
        }
        if (r < 0) {
            fprintf(stderr, "error from select: %s\n", strerror(errno));
            g_usleep(1000*1000);
            continue;
        }

        hab_read();
    }

    close(urandom_fd);

    return 0;
}




void destroy_node(bionet_hab_t* random_hab) {
    bionet_node_t *node;

    node = pick_random_node(random_hab);
    if (node == NULL) return;

    if (output_mode == OM_NORMAL) printf("removing Node %s\n", bionet_node_get_id(node));
    else if (output_mode == OM_BIONET_WATCHER) printf("lost node: %s\n", bionet_node_get_name(node));

    bionet_hab_remove_node_by_id(random_hab, bionet_node_get_id(node));
    hab_report_lost_node(bionet_node_get_id(node));
    bionet_node_free(node);
}

