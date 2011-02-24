
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


    //This is syshealth written in c instead of shell
    //Written by Marek Sotola (with lots of help from others) //

    //  Updated to use Bionet instead of the parameter server (Seb)

    //
    //This is a program that gathers system info (MB free, KB memory free, 
    //seconds of uptime, and 15 minute load average) and feeds it into
    // Bionet, for webplotting on the ground.
    //


#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <getopt.h>

#include <glib.h>
#include <hardware-abstractor.h>

#include "syshealth.h"




typedef struct {
    char *name;
    int enabled;
    int (*init)(bionet_node_t *node);
    void (*update)(bionet_node_t *node);
} input_module_t;

input_module_t input[] = {
    { .name = "SMART",               .enabled = 1,  .init = smart_init,               .update = smart_update              },
    { .name = "disk-free",           .enabled = 1,  .init = disk_free_init,           .update = disk_free_update          },
    { .name = "mem-free",            .enabled = 1,  .init = mem_free_init,            .update = mem_free_update           },
    { .name = "uptime",              .enabled = 1,  .init = uptime_init,              .update = uptime_update             },
    { .name = "load-average",        .enabled = 1,  .init = load_average_init,        .update = load_average_update       },
    { .name = "sensors",             .enabled = 1,  .init = sensors_init,             .update = sensors_update            },
    { .name = "scheduling-latency",  .enabled = 1,  .init = scheduling_latency_init,  .update = scheduling_latency_update },
    { .name = "RAID",                .enabled = 1,  .init = raid_init,                .update = raid_update },
    { .name = NULL,                  .enabled = 1,  .init = NULL,                     .update = NULL                      }
};



char syshealth_pid_file_name[]="/tmp/syshealth.pid";




void usage(FILE * fp) {
    fprintf(fp,
	    "'syshealth-hab' reports system health information to Bionet\n"
	    "\n"
	    "usage: syshealth-hab [OPTIONS]\n"
	    "\n"
	    "-d,--delay <sec>         Wait N seconds between updates.  Default is 300\n"
	    "-i,--info                Publish Bionet library information\n"
	    "-h,--help                Show this help and exit\n"
	    "-s,--security-dir <dir>  Directory containing security certificates\n"
	    "-v,--version             Show the version number\n");
}




int main(int argc, char **argv) {
    long int seconds_to_sleep = 300;

    bionet_node_t *node;
    bionet_hab_t *hab;

    int bionet_fd;
    char * security_dir = NULL;
    int publish_info = 0;

    bionet_log_context_t log_context = {
        .destination = BIONET_LOG_TO_SYSLOG,
        .log_limit = G_LOG_LEVEL_INFO
    };


    bionet_log_use_default_handler(&log_context);


    // 
    // check command-line arguments
    //
    int i;
    int c;
    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"verison", 0, 0, 'v'},
	    {"delay", 1, 0, 'd'},
	    {"security-dir", 1, 0, 's'},
	    {"info", 0, 0, 'i'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?vhid:s:", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {

	case '?':
	case 'h':
	    usage(stdout);
	    return 0;

	case 'd':
	    seconds_to_sleep = strtol(optarg, NULL, 0);
	    if (LONG_MIN == seconds_to_sleep || LONG_MAX == seconds_to_sleep) {
		g_log("", G_LOG_LEVEL_ERROR, "Failed to convert delay: %m");
		return 1;
	    }
	    break;

	case 'i':
	    publish_info = 1;
	    break;

	case 's':
	    security_dir = optarg;
	    break;
	    
	case 'v':
	    print_bionet_version(stdout);
	    return 0;

	default:
	    break;
	}
    }

    if (security_dir) {
	if (hab_init_security(security_dir, 1)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
	}
    }

    // 
    // Create & connect the Hab
    //

    { 
        hab = bionet_hab_new("syshealth", NULL);
        if (hab == NULL) {
            g_log("", G_LOG_LEVEL_ERROR, "error creating the hab");
            exit(1);
        }

        hab_register_callback_set_resource(NULL);

        bionet_fd = hab_connect(hab);
        if (bionet_fd < 0) {
            g_log("", G_LOG_LEVEL_ERROR, "hab_connect error");
            exit(1);
        }
    }

    // 
    // make the Node and add the Resources
    //

    node = bionet_node_new(hab, "system");
    if (node == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "error creating new node");
        exit(1);
    }

    {
        int i;

        for (i = 0; input[i].init != NULL; i ++) {
            if (input[i].enabled == 0) {
                continue;
            }
            if (input[i].init(node) < 0) {
                g_log("", G_LOG_LEVEL_WARNING, "input module '%s' init failed, disabling", input[i].name);
                input[i].enabled = 0;
            }
        }
    }


    // 
    // report the new Node 
    // 
    if (publish_info) {
	if (hab_publish_info(hab, BIONET_INFO_VERSION_FLAG)) {
	    g_log("", G_LOG_LEVEL_ERROR, "hab error adding a node!");
	}
    }
    if (bionet_hab_add_node(hab, node)) g_log("", G_LOG_LEVEL_ERROR, "hab error adding a node!");
    if (hab_report_new_node(node)) g_log("", G_LOG_LEVEL_ERROR, "hab error reporting a new node!");
    if (hab_report_datapoints(node)) g_log("", G_LOG_LEVEL_ERROR, "hab error reporting datapoints!");


    g_log("", G_LOG_LEVEL_INFO, "Starting Program syshealth");


    //
    // main loop
    //

    while (1) {
        int i, r;
        fd_set readers;
        struct timeval start, now;
        int time_spent_sleeping = 0;

        for (i = 0; input[i].update != NULL; i ++) {
            if (input[i].enabled) {
                input[i].update(node);
            }
        }

        if (hab_report_datapoints(node)) 
            g_log("", G_LOG_LEVEL_WARNING, "error updating!!");

        r = gettimeofday(&start, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "gettimeofday error: %s", strerror(errno));
            continue;
        }

        do {
            struct timeval tv;

            tv.tv_sec = seconds_to_sleep - time_spent_sleeping;
            tv.tv_usec = 0;

            FD_ZERO(&readers);
            FD_SET(bionet_fd, &readers);

            r = select(bionet_fd + 1, &readers, NULL, NULL, &tv);
            if (FD_ISSET(bionet_fd, &readers)) {
                hab_read();
            } else if (r < 0) {
                g_log("", G_LOG_LEVEL_WARNING, "select error");
                break;
            }
            
            r = gettimeofday(&now, NULL);
            if (r < 0) {
                g_log("", G_LOG_LEVEL_WARNING, "gettimeofday error: %s", strerror(errno));
                continue;
            }

            time_spent_sleeping = now.tv_sec - start.tv_sec;

        } while (time_spent_sleeping < seconds_to_sleep);
    }
    

    // NOT REACHED
    return 0;
}

