
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <sys/types.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cpod.h"
#include "hardware-abstractor.h"


int daemonize(int*);


// In pidfile.c
int kill_kin();
int write_pidfile();


void print_help(char* name)
{
    printf("Usage: %s [options] bdaddr\n", name);
    printf("Options are the following:\n");
    printf("-d,--daemon\t\tRun program as a daemon\n");
    printf("-e,--ecg\t\tParse and report the ECG output\n");
    printf("-h,--hab-id name\tSets the hab id\n");
    printf("-?,--help\t\tPrint this help message\n");
    printf("-k,--kill\t\tSearch for a running instance of processs and kill it\n");
    printf("-p,--period time\tPeriod between battery queries in seconds\n");
    printf("-v,--verbose\t\tTurn on verbose logging\n");
}

bionet_node_t *node;
bionet_hab_t *hab;

int print_ecg;


int main ( int argc, char** argv )
{
    int cpod_fd, i, verbose, hab_fd, period, streaming, daemon;

    bdaddr_t device;

    struct sockaddr_rc addr = {0};

    char *hab_id = NULL;

    daemon = 0;
    cpod_fd = -1;
    print_ecg = 0;
    verbose = 0;
    streaming = 0;
    bacpy(&device, BDADDR_ANY);
    period = 10;


    //  Parse command line arguments
    for(i = 1; i < argc; i++ )
    {
        if (strcmp(argv[i], "--verbose") == 0 ||
            strcmp(argv[i], "-v") == 0)
        {
            verbose = 1;
        }
        else if (strcmp(argv[i], "--daemon") == 0 ||
                 strcmp(argv[i], "--demon") == 0 ||
                 strcmp(argv[i], "-d") == 0)
        {
            daemon = 1;
        }
        else if (strcmp(argv[i], "--ecg") == 0 ||
                 strcmp(argv[i], "-e") == 0)
        {
            print_ecg = 1;
        }
        else if (strcmp(argv[i], "--help") == 0 ||
                 strcmp(argv[i], "-?") == 0)
        {
            print_help(argv[0]);
            exit(0);
        }
        else if (strcmp(argv[i], "--kill") == 0 ||
                 strcmp(argv[i], "-k") == 0)
        {
            int r;

            if ((r = kill_kin()))
                g_log("", G_LOG_LEVEL_WARNING, "Can't kill process: %s", strerror(errno));

            return r;
        }
        else if (strcmp(argv[i], "--hab-id") == 0 ||
                 strcmp(argv[i], "-h") == 0)
        {
            i++;

            if (i < argc)
                hab_id = argv[i];
            else
            {
                print_help(argv[0]);
                exit(2);
            }
        }
        else if (strcmp(argv[i], "--period") == 0 ||
                 strcmp(argv[i], "-p") == 0)
        {
            i++;

            if ((i < argc) &&
		(0 != (period = strtol(argv[i], NULL, 0))) ||
		((0 == period) && (EINVAL != errno)))
            { }
            else
            {
                print_help(argv[0]);
                exit(2);
            }
        }
        else if (strlen(argv[i]) == 17 &&
                 str2ba(argv[i], &device) == 0 &&
                 bacmp(&device, BDADDR_ANY) != 0)
        {
                g_log("", G_LOG_LEVEL_DEBUG, "Connecting to %s", argv[i]);
        }
        else
        {
            print_help(argv[0]);
            exit(2);
        }
    }

    if (daemon)
    {
        if (daemonize(&verbose) < 0)
        {
            g_log("", G_LOG_LEVEL_CRITICAL, "Can't daemonize - Aborting");
            exit(1);
        }

        if (write_pidfile() != 0)
            g_log("", G_LOG_LEVEL_CRITICAL, "Can't write pidfile: %s - Continuing...", strerror(errno));

    }



    //  Create default device...
    if (bacmp(&device, BDADDR_ANY) == 0)
        str2ba(CPOD_BDADDR, &device);

    // Initialize the CRC-CCITT calculation table
    init_crcccitt_table();

    // Create the bionet hab
    hab = bionet_hab_new(HAB_TYPE, hab_id);
    if (hab == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "Unable to create a new hab...");
        exit(1);
    }

    //  Connect
connect_to_bionet:

    hab_fd = hab_connect(hab);
    if (hab_fd == -1)
    {
        g_critical("Can't connect to bionet: %s - Restarting...", strerror(errno));
        g_usleep(10 * 1000*1000);
        goto connect_to_bionet;
    }


     // Open the bluetooth device
connecting:
    if (cpod_fd == -1)
    {
        cpod_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

        addr.rc_family = AF_BLUETOOTH;
        addr.rc_channel = (uint8_t) 1;
        str2ba(CPOD_BDADDR, &addr.rc_bdaddr);

        g_log("", G_LOG_LEVEL_DEBUG, "Connecting to CPOD...");

        if (connect(cpod_fd, (struct sockaddr *) &addr, sizeof(addr)))
        {
            g_debug("Can't connect to the cpod device, fd = %d: %s", cpod_fd, strerror(errno));
            g_usleep(3*1000*1000);
            close(cpod_fd);
            cpod_fd = -1;
            goto connecting;
        }
        g_debug("Connected.");
    }

    // Report resources
    i = 0;

    node = bionet_node_new(hab, NODE_ID);
    if (node == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "error creating node, quitting");
        return 1;
    }
    if (bionet_hab_add_node(hab, node) != 0) {
        g_log("", G_LOG_LEVEL_ERROR, "error adding node to hab, quitting");
        return 1;
    }


    if (print_ecg)
    {
        i += create_zeroed_float_sensor(node, "ECG-II");
        i += create_zeroed_float_sensor(node, "ECG-V5");
    }

    /*i += create_zeroed_float_sensor(node, "Respiration-Raw");*/
    i += create_zeroed_float_sensor(node, "Accel-X");
    i += create_zeroed_float_sensor(node, "Accel-Y");
    i += create_zeroed_float_sensor(node, "Accel-Z");
    i += create_zeroed_float_sensor(node, "Temperature");
    i += create_zeroed_float_sensor(node, "Pulse-Oximetry");
    i += create_zeroed_float_sensor(node, "Heart-Rate");

    i += hab_report_new_node(node);

    if (node == NULL ||
        i != 0)
    {
        g_critical("Can't report node - trying again");
        close(hab_fd);
        close(cpod_fd);
        cpod_fd = -1;
        bionet_hab_remove_node_by_id(hab, NODE_ID);
        hab_report_lost_node(NODE_ID);
        bionet_node_free(node);
        goto connecting;
    }


    // Start the sequence...
    if ((i = start_streaming(cpod_fd))    ||
        (i = available_opcodes(cpod_fd))  ||
        (i = set_time(cpod_fd)))
    {
        g_warning("Can't initialize the cpod device: %d - Restarting", i);
        close(cpod_fd);
        cpod_fd = -1;
        bionet_hab_remove_node_by_id(hab, NODE_ID);
        hab_report_lost_node(NODE_ID);
        bionet_node_free(node);
        g_usleep(3*1000*1000);
        goto connecting;
    }


    fcntl(cpod_fd, F_SETFL, O_NONBLOCK);

    reset_bookkeeping();

    send_next_packet_streaming(cpod_fd);


    for (;;)
    {
        fd_set readers;
        struct timeval timeout = {period, 0};
        int max_fd;

        FD_ZERO(&readers);

        FD_SET(hab_fd, &readers);
        FD_SET(cpod_fd, &readers);
        max_fd = MAX(hab_fd, cpod_fd);


        i = select(max_fd + 1, &readers, NULL, NULL, &timeout);


        if (i < 0)
        {
            g_critical("Error from select: %s - Restarting\n", strerror(errno));
            close(cpod_fd);
            cpod_fd = -1;

            bionet_hab_remove_node_by_id(hab, NODE_ID);
            hab_report_lost_node(NODE_ID);
            bionet_node_free(node);

            g_usleep(3*1000*1000);
            goto connecting;
        }
        else if (i == 0)
        {
            static int retrys = 0;

            retrys++;

            if(retrys != 5)
                send_next_packet_streaming(cpod_fd);
            else
            {
                retrys = 0;
                close(cpod_fd);
                cpod_fd = -1;

                bionet_hab_remove_node_by_id(hab, NODE_ID);
                hab_report_lost_node(NODE_ID);
                bionet_node_free(node);

                goto connecting;
            }
        }


        if (FD_ISSET(hab_fd, &readers))
            hab_read();


        if (FD_ISSET(cpod_fd, &readers))
        {
            int r;

            r = read_next_packet(cpod_fd);
            if (r == -1)
            {
                g_critical("Error reading from cpod: %s - Restarting", strerror(errno));
                close(cpod_fd);
                cpod_fd = -1;

                bionet_hab_remove_node_by_id(hab, NODE_ID);
                hab_report_lost_node(NODE_ID);
                bionet_node_free(node);

                g_usleep(3*1000*1000);
                goto connecting;
            }
            else if (r == 1)
            {
                g_usleep(10*1000);
                send_next_packet_streaming(cpod_fd);
            }
        }
    }

    return 0;
}
