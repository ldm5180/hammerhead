
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

/**
 * @file
 * @brief Mini meteorite detection HAB
 *
 * Main HAB loop for communications from the NAG and from the Motes
 *
 * Rationale: Provide an abstracted means of communicating between Motes and
 * NAG clients
 */

#include <sys/types.h>
#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gtimer.h>
#include <unistd.h>

#include "hardware-abstractor.h"
#include "mmod.h"
#include "heartbeat.h"
#include "gateway.h"
#include "serialsource.h"
#include "daemonize.h"


static void parse_cmdline(int argc, char** argv);
static void print_usage(FILE* fout);

static int verbose = 0;
static int daemon_mode = 0;
static char * usb_dev = DEFAULT_USB_DEV;

uint16_t heartbeat_time = DEFAULT_HEARTBEAT_TIME;
serial_source gw_src;
bionet_hab_t * this;


int main(int argc, char** argv)
{
    int gw_fd;
    int bionet_fd;

    /* do some HAB setup */
    this = bionet_hab_new(MMOD_HAB_TYPE, NULL);
    if (NULL == this)
    {
	fprintf(stderr, "Failed to get a new hab\n");
	return(1);
    }

    parse_cmdline(argc, argv);

    /* daemonize if requested */
    if (daemon_mode)
    {
	daemonize(&verbose);
    }

    bionet_fd = hab_connect(this);
    if (0 > bionet_fd)
    {
	fprintf(stderr, "Failed to connect HAB to Bionet\n");
	return(bionet_fd);
    }

    pause();
    /* connect to the gateway mote */
    gw_src = gateway_open(usb_dev);
    if (NULL == gw_src)
    {
	fprintf(stderr, "Failed to open the gateway device %s\n", usb_dev);
	return (1);
    }
    gw_fd = serial_source_fd(gw_src);
    
    /* set the resource callback */
    hab_register_callback_set_resource(cb_set_resource);

    /* every time a message is rvcd from a mote, check mote is already a node.
     * if not, add it and its resources. otherwise just send the values along.
     * every time a message is rcvd from the NAG, forward it along to motes */
    while(1)
    {
	fd_set readers;
	struct timeval timeout;
	int max_fd;
	int ret;
	int num_read;

	timeout.tv_sec = heartbeat_time;
	timeout.tv_usec = 0;

	FD_ZERO(&readers);
	FD_SET(bionet_fd, &readers);
	FD_SET(gw_fd, &readers);
	max_fd = MAX(bionet_fd, gw_fd);

	/* now select on sockets and do the right thing when a ship comes in.
	 *
	 * No retry logic is needed because this is the main loop anyway. Just
	 * make sure that errors are handled correctly. */
	ret = select(max_fd + 1, &readers, NULL, NULL, &timeout);
	if (0 > ret)
	{ 
	    if ((EAGAIN != errno)
		&& (EINTR != errno))
	    {
		g_critical("Error from select: %s\n", strerror(errno));
		
		/* either EBADF, EINVAL, or ENOMEM so sleep for a while */
		sleep(5);
	    }
	}
	else if (0 < ret)
	{
	    if (FD_ISSET(bionet_fd, &readers))
	    {
		hab_read();
	    }
	    
	    if (FD_ISSET(gw_fd, &readers))
	    {
		num_read = gateway_read(gw_src);
		/* is the number of messages read even relevant? */
	    }
	}
	
	/* check all attached nodes to ensure that 2*heartbeat_period has
	 * not elapsed. if it has then report lost nodes */
	heartbeat_check();
    }

} /* main() */


/**
 * @brief Parse the command line arguments
 *
 * @param[in] argc Number of arguments on the command line
 * @param[in] argv Array of arguments from the command line 
 */
static void parse_cmdline(int argc, char** argv)
{
    const char * optstring = "dh?v:t:i:u:";
    int opt;

    while (0 < (opt = getopt(argc, argv, optstring)))
    {
	switch (opt)
	{
	case 'd':
	    daemon_mode = 1;
	    break;
	    
	case 'h':
	case '?':
	    print_usage(stdout);
	    exit(0);
	    break;

	case 'i':
	    bionet_hab_set_id(this, optarg);
	    break;
	    
	case 't':
	    heartbeat_time = strtol(optarg, NULL, 0);
	    break;
	    
	case 'u':
	    usb_dev = strdup(optarg);
	    break;

	case 'v':
	    verbose++;
	    break;

	default:
	    print_usage(stderr);
	    exit(1);
	    break;
	}
    }
} /* parse_cmdline() */


/**
 * @brief Print the valid command line options
 *
 * @param[in] fout File ptr to which the usage shall be written. Common use
 * is stderr or stdout.
 */
static void print_usage(FILE* fout)
{
    fprintf(fout,
	    "Usage: mmod-hab [-v] [-d] [-h] [-?] [-i id] [-t sec] [-u dev]\n"
	    "    -d     run as a daemon\n"
	    "    -h -?  display this help\n"
	    "    -i     HAB ID (mmod)\n"
	    "    -t     seconds between required mote heartbeats\n"
	    "    -u     USB serial device (/dev/ttyUSB1)"
	    "    -v     verbose logging\n");
} /* print_usage() */
