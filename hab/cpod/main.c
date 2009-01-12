
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//


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
    printf("=======================================================================================================\n");
    printf("bdaddr\t\t\tBDADDR of the device you'd like to connect to\n\n");
    printf("Options are the following:\n");
    printf("-d,--daemon\t\tRun program as a daemon\n");
    printf("-e,--ecg\t\tParse and report the ECG output\n");
    printf("-h,--hab-id name\tSets the hab id\n");
    printf("-?,--help\t\tPrint this help message\n");
    printf("-k,--kill\t\tSearch for a running instance of processs and kill it\n");
    printf("-n,--nag hostname\tThe hostname of the NAG to connect to\n");
    printf("-p,--period time\tPeriod between battery queries in seconds\n");
    printf("-v,--verbose\t\tTurn on verbose logging\n");
}


bionet_node_t *node;

int print_ecg;


int main ( int argc, char** argv )
{
    int cpod_fd, i, verbose, nag_fd, period, streaming, daemon;

    bdaddr_t device;

    struct sockaddr_rc addr = {0};


    daemon = 0;
    cpod_fd = -1;
    print_ecg = 0;
    verbose = 0;
    streaming = 0;
    bacpy(&device, BDADDR_ANY);
    period = 10;

    hab_set_nag_hostname(DEFAULT_NAG);
    hab_set_type(HAB_TYPE);


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
        else if (strcmp(argv[i], "--nag") == 0 ||
                 strcmp(argv[i], "-n") == 0)
        {
            i++;

            if (i < argc)
            {
                hab_set_nag_hostname(argv[i]);
            }
            else
            {
                print_help(argv[0]);
                exit(2);
            }
        }
        else if (strcmp(argv[i], "--hab-id") == 0 ||
                 strcmp(argv[i], "-h") == 0)
        {
            i++;

            if (i < argc)
                hab_set_id(argv[i]);
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
                (sscanf(argv[i], "%d", &period) == 1))
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


    //  Connect to the nag
connect_to_nag:
    nag_fd = hab_connect_to_nag();

    if (nag_fd == -1)
    {
        g_critical("Can't connect to nag: %s - Restarting...", strerror(errno));
        g_usleep(10 * 1000*1000);
        goto connect_to_nag;
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

    // Report resources to nag
    i = 0;

    node = bionet_node_new(NULL, NULL, NODE_ID);

    if (print_ecg)
    {
        i += bionet_node_add_resource_with_valuestr_timestr(
            node,
            "Float",
            "Sensor",
            "ECG-II",
            "0",
            NULL);

        i += bionet_node_add_resource_with_valuestr_timestr(
            node,
            "Float",
            "Sensor",
            "ECG-V5",
            "0",
            NULL);
    }

    /*i += bionet_node_add_resource_with_valuestr_timestr(
        node,
        "Float",
        "Sensor",
        "Respiration-Raw",
        "0",
        NULL);*/

    i += bionet_node_add_resource_with_valuestr_timestr(
        node,
        "Float",
        "Sensor",
        "Accel-X",
        "0",
        NULL);

    i += bionet_node_add_resource_with_valuestr_timestr(
        node,
        "Float",
        "Sensor",
        "Accel-Y",
        "0",
        NULL);

    i += bionet_node_add_resource_with_valuestr_timestr(
        node,
        "Float",
        "Sensor",
        "Accel-Z",
        "0",
        NULL);

    i += bionet_node_add_resource_with_valuestr_timestr(
        node,
        "Float",
        "Sensor",
        "Temperature",
        "0",
        NULL);

    i += bionet_node_add_resource_with_valuestr_timestr(
        node,
        "Float",
        "Sensor",
        "Pulse-Oximetry",
        "0",
        NULL);

    i += bionet_node_add_resource_with_valuestr_timestr(
        node,
        "Float",
        "Sensor",
        "Heart-Rate",
        "0",
        NULL);


    i += hab_report_new_node(node);

    if (node == NULL ||
        i != 0)
    {
        g_critical("Can't report node to nag - Restarting");
        close(nag_fd);
        close(cpod_fd);
        cpod_fd = -1;
        goto connect_to_nag;
    }


    // Start the sequence...
    if ((i = start_streaming(cpod_fd))    ||
        (i = available_opcodes(cpod_fd))  ||
        (i = set_time(cpod_fd)))
    {
        g_warning("Can't initialize the cpod device: %d - Restarting", i);
        close(cpod_fd);
        cpod_fd = -1;
        g_usleep(3*1000*1000);
        goto connecting;
    }


    fcntl(cpod_fd, F_SETFL, O_NONBLOCK);


    send_next_packet_streaming(cpod_fd);


    for (;;)
    {
        fd_set readers;
        struct timeval timeout = {period, 0};
        int max_fd;

        FD_ZERO(&readers);

        FD_SET(nag_fd, &readers);
        FD_SET(cpod_fd, &readers);
        max_fd = MAX(nag_fd, cpod_fd);


        i = select(max_fd + 1, &readers, NULL, NULL, &timeout);


        if (i < 0)
        {
            g_critical("Error from select: %s - Restarting\n", strerror(errno));
            close(cpod_fd);
            cpod_fd = -1;

            hab_report_lost_node(NODE_ID);

            g_usleep(3*1000*1000);
            goto connect_to_nag;
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

                hab_report_lost_node(NODE_ID);

                goto connecting;
            }
        }


        if (FD_ISSET(nag_fd, &readers))
            hab_read_from_nag();

        hab_handle_queued_nag_messages();


        if (FD_ISSET(cpod_fd, &readers))
        {
            int r;

            r = read_next_packet(cpod_fd);
            if (r == -1)
            {
                g_critical("Error reading from cpod: %s - Restarting", strerror(errno));
                close(cpod_fd);
                cpod_fd = -1;

                hab_report_lost_node(NODE_ID);

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
