
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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>


#include "amedevice.h"
#include "hardware-abstractor.h"


#define MAX(a, b)  (((a) > (b)) ? (a) : (b))


#define STETHOSCOPE_BDADDR "00:04:3E:05:20:F4"
#define STETHOSCOPE_NAME   "AME BTSTETH"
#define DEFAULT_NAG        "bionet-basestation.colorado.edu"
#define HAB_TYPE           "Stethoscope"

#define NODE_ID            "00-04-3E-05-20-F4"


uint32_t gain;

bionet_node_t *node;

typedef struct {
    int socket;
} user_data_t;

int num_listeners = 0;

int daemonize(int*);

// In pidfile.c
int kill_kin();

// In pidfile.c
int write_pidfile();

void cleanup_children()
{
    g_log("", G_LOG_LEVEL_INFO, "Got a child signal...");
}

int read_data_from_stethoscope_and_write(int fd, bionet_stream_t * stream, int num_listeners);

void cb_set_resource(bionet_resource_t * resource, bionet_value_t * value);
void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream);
void cb_stream_unsubscription(const char *client_id, const bionet_stream_t *stream);
void cb_stream_data(const char *client_id, bionet_stream_t *stream, const void *data, unsigned int size);

void print_help(char* name)
{
    printf("Usage: %s [options] bdaddr\n", name);
    printf("=======================================================================================================\n");
    printf("bdaddr\t\t\tBDADDR of the device you'd like to connect to\n\n");
    printf("Options are the following:\n");
    printf("-d,--daemon\t\tBackground process as daemon\n");
    printf("-g,--gain x\t\tSets the amplifier gain to integer x\n");
    printf("-h,--hab-id name\tSets the hab id\n");
    printf("-?,--help\t\tPrint this help message\n");
    printf("-k,--kill\t\tSearch for a running instance of processs and kill it\n");
    printf("-p,--period time\tPeriod between battery queries in seconds\n");
    printf("-v,--verbose\t\tTurn on verbose logging\n");
}




int main(int argc, char** argv)
{
    int i, daemon, verbose, s, bionet_fd, period, streaming;

    struct sigaction sa;
    struct sockaddr_rc addr = { 0 };

    bdaddr_t device, interface;

    bionet_stream_t *steth_stream;

    char * hab_id = NULL;
    bionet_hab_t * this_hab;
    bionet_resource_t * resource;

    verbose = 0;
    daemon = 0;
    streaming = 0;
    bacpy(&device, BDADDR_ANY);
    gain = 10;
    period = 10;


    // Parse command line arguments
    for (i = 1; i < argc; i++)
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
        else if (strcmp(argv[i], "--gain") == 0 ||
                 strcmp(argv[i], "-g") == 0)
        {
            i++;

            if ((i < argc) && (sscanf(argv[i], "%d", &gain) == 1)) {
                ; // ok
                // g_message("Set the gain to %d", gain);
            }
            else
            {
                print_help(argv[0]);
                exit(2);
            }
        }
        else if (strcmp(argv[i], "--kill") == 0 ||
                 strcmp(argv[i], "-k") == 0)
        {
            int r;

            if ((r = kill_kin()))
                g_log("", G_LOG_LEVEL_WARNING, "Can't kill process: %s", strerror(errno));

            return r;
        }
        else if (strcmp(argv[i], "--help") == 0 ||
                 strcmp(argv[i], "-?") == 0)
        {
            print_help(argv[0]);
            exit(0);
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

    //  Create default device...
    if (bacmp(&device, BDADDR_ANY) == 0)
    {
        str2ba(STETHOSCOPE_BDADDR, &device);
        //print_help(argv[0]);
        //exit(2);
    }

    this_hab = bionet_hab_new(HAB_TYPE, hab_id);
    if (NULL == this_hab) {
	g_log("", G_LOG_LEVEL_CRITICAL, "Failed to get a new HAB");
	exit (1);
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



    // Setup children handling
    sa.sa_handler = cleanup_children;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) < 0)
        return -3;



    bacpy(&interface, BDADDR_ANY);


    // Get RFCOMM channel of device via SDP
    // Naaaa...Just use channel 1 til we get millions to rewrite it

    hab_register_callback_set_resource(cb_set_resource);
    hab_register_callback_stream_data(cb_stream_data);
    hab_register_callback_stream_subscription(cb_stream_subscription);
    hab_register_callback_stream_unsubscription(cb_stream_unsubscription);

    // first connect to Bionet to show we're alive
    bionet_fd = hab_connect(this_hab);
    if (bionet_fd == -1)
    {
        g_log("", G_LOG_LEVEL_CRITICAL, 
	      "Can't connect to Bionet: %s - Aborting...", 
	      strerror(errno));
        exit(1);
    }


    // now, connect to device
    g_log("", G_LOG_LEVEL_INFO, "trying to connect to stethoscope device");

connecting:
    do {
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (0 > s) {
	    g_log("", G_LOG_LEVEL_ERROR, "failed to open Bluetooth socket: %m");
	    sleep(1);
	    g_log("", G_LOG_LEVEL_DEBUG, "trying socket() again...");
	}
    } while (0 > s);

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) AME_CHANNEL_NUMBER;
    bacpy(&addr.rc_bdaddr, &device);


    while (connect(s, (struct sockaddr *)&addr, sizeof(addr)))
    {
        // g_critical("Waiting for device to show up...: %s", strerror(errno));
        g_usleep(3*1000*1000);
        close(s);
        goto connecting;
    }


    // Verify this is a stethoscope...
    {
        char *name = NULL;

        while (name == NULL)
	{
	    ame_command_return_name(s);
	    name = ame_response_return_name(s);
	    g_debug("Read name: >%s<", name);
	    if ((name) && (0 == strcmp(name, STETHOSCOPE_NAME)))
	    {
		break;
	    }
	    g_warning("Can't verify device as Stethoscope device, reports %s - Continuing...", name);
	    name = NULL;
	}
    }


    // Report battery voltage and audio stream to bionet
    node = bionet_node_new(this_hab, NODE_ID);

    steth_stream = bionet_stream_new(node, "output", BIONET_STREAM_DIRECTION_PRODUCER, "audio");
    if (steth_stream == NULL) {
        g_critical("out of memory!\n");
        exit(1);
    }

    bionet_stream_set_user_data(steth_stream, calloc(1, sizeof(user_data_t)));
    
    user_data_t * user_data = (user_data_t *)bionet_stream_get_user_data(steth_stream);
    user_data->socket = s;

    i = 0;

    i += bionet_node_add_stream(node, steth_stream);

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_FLOAT,
				   BIONET_RESOURCE_FLAVOR_SENSOR,
				   "Battery-Voltage");
    if (NULL == resource) {
	g_warning("Unable to new the Battery-Voltage resource");
    }
    i += bionet_node_add_resource(node, resource);

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_UINT32,
				   BIONET_RESOURCE_FLAVOR_PARAMETER,
				   "Gain");
    if (NULL == resource) {
	g_warning("Unable to new the Gain resource");
    }
    i+= bionet_node_add_resource(node, resource);

    bionet_resource_set_uint32(resource, gain, NULL);

    bionet_hab_add_node(this_hab, node);
    i += hab_report_new_node(node);

    if (node == NULL ||
        steth_stream == NULL ||
        i != 0)
    {
        g_critical("Can't report node to Bionet - Aborting");
        exit(1);
    }



    // Make the device socket nonblocking

    fcntl(s, F_SETFL, O_NONBLOCK);


    while (1)
    {
        fd_set readers;
        struct timeval timeout = {period, 0};
        int max_fd;

        FD_ZERO(&readers);

        FD_SET(bionet_fd, &readers);
        FD_SET(s, &readers);
        max_fd = MAX(bionet_fd, s);


        i = select(max_fd + 1, &readers, NULL, NULL, &timeout);
        if (i < 0)
        {
            g_critical("Error from select: %s - Aborting\n", strerror(errno));
            exit(1);
        }
        else if (i == 0)
        {
            //  Timeout - query the battery.
            if (!streaming)
                ame_command_return_battery_status(s);
            continue;
        }


        if (FD_ISSET(bionet_fd, &readers))
            hab_read();


        if (FD_ISSET(s, &readers))
        {
            if (read_data_from_stethoscope_and_write(s, steth_stream, num_listeners))
            {
                g_message("Lost conection to stethoscope: %s - Resetting\n", strerror(errno));

                close(s);

                hab_report_lost_node(NODE_ID);

                streaming = 0;

                goto connecting;
            }

	    if ((num_listeners) && (!streaming))
	    {
		
		g_debug("Accepted a connection on the Stethoscope stream.");
		ame_command_return_data(s, SAMPLE_RATE_RESYNCH, 0);
		streaming = 1;
	    }
	    
	    if ((0 == num_listeners) && (streaming))
	    {
		//turn off streaming
		streaming = 0;
	    }
	}
    }


    return 0;
}
