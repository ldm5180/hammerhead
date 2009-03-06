
// Copyright (c) 2008-2009, Regents of the University of Colorado.
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
    { .name = NULL,                  .enabled = 1,  .init = NULL,                     .update = NULL                      }
};



char syshealth_pid_file_name[]="/tmp/syshealth.pid";




void usage(void) {
    printf("usage: syshealth-hab [OPTIONS]\n\
OPTIONS:\n\
    --delay N    Wait N seconds between updates.  Default is 30.\n\
    --help       Show this help and exit.\n\
");
}




int main(int argc, char **argv) {
    int seconds_to_sleep = 300;

    bionet_node_t *node;
    bionet_hab_t *hab;

    int bionet_fd;


    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_SYSLOG,
        log_limit: G_LOG_LEVEL_INFO
    };


    g_log_set_default_handler(bionet_glib_log_handler, &log_context);


    // 
    // check command-line arguments
    //
    
    {
        int i;

        for (i = 1; i < argc; i ++) {

            if (strcmp(argv[i], "--delay") == 0) {
                i ++;
                seconds_to_sleep = atoi(argv[i]);

            } else if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
                usage();
                exit(0);

            } else {
                printf("unknown command-line argument: %s", argv[i]);
                usage();
                exit(1);
            }
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
    // report the new Node to the NAG
    // 

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

