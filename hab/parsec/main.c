
//
// Copyright (C) 2009, Regents of the University of Colorado.
//

/**
 * @file
 * @brief Parsec HAB
 *
 * Main HAB loop for communicating with the PARSEC
 *
 */

#include <arpa/inet.h>
#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "hardware-abstractor.h"
#include "daemonize.h"

static void parse_cmdline(int argc, char** argv);
static void print_usage(char* prog_name, FILE* fout);
void read_parsec(int fd);

static const char* hab_type = "parsec";

static int verbose = 0;
static int daemon_mode = 0;
static uint16_t port = 61557;
bionet_hab_t * parsec_hab;
char * parsec_id = NULL;


int main(int argc, char** argv) {
    int parsec_fd;
    int bionet_fd;
	struct sockaddr_in server;


    parse_cmdline(argc, argv);

    // do some HAB setup
    parsec_hab = bionet_hab_new(hab_type, parsec_id);
    if (NULL == parsec_hab) {
        fprintf(stderr, "Failed to maje a new hab\n");
	    return(1);
    }

    // daemonize if requested
    if (daemon_mode) {
        daemonize(&verbose);
    }

    bionet_fd = hab_connect(parsec_hab);
    if (bionet_fd < 0)
    {
        g_critical("Failed to connect HAB to Bionet\n");
        return(2);
    }

    // open UDP port

	parsec_fd = socket(AF_INET, SOCK_DGRAM, 0);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	if (bind(parsec_fd, (struct sockaddr*)&server, sizeof(server)) != 0) {
        g_critical("Can't bind to port %hd, quitting./n", port);
        exit(3);	
	}

        
    while(1) {
        fd_set readers;
        int max_fd;
		int ret;

		FD_ZERO(&readers);
		FD_SET(bionet_fd, &readers);
		FD_SET(parsec_fd, &readers);
		max_fd = MAX(bionet_fd, parsec_fd);

		ret = select(max_fd + 1, &readers, NULL, NULL, NULL);
		if (ret < 0) { 
			if ((EAGAIN != errno) && (EINTR != errno)) {
				g_critical("Error from select: %s\n", strerror(errno));
		
				/* either EBADF, EINVAL, or ENOMEM so sleep for a while */
				exit(4);
			}
		}
		else if (ret > 0) {
			if (FD_ISSET(bionet_fd, &readers)) {
				hab_read();
			}

			if (FD_ISSET(parsec_fd, &readers)) {
				read_parsec(parsec_fd);
			}
		}
    }
    return 0;
} /* main() */


/**
 * @brief Parse the command line arguments
 *
 * @param[in] argc Number of arguments on the command line
 * @param[in] argv Array of arguments from the command line 
 */
static void parse_cmdline(int argc, char** argv)
{
    const char * optstring = "dh?vi:p:";
    int opt;

    while (0 < (opt = getopt(argc, argv, optstring))) {
		switch (opt) {
		case 'd':
			daemon_mode = 1;
			break;
	    
		case 'h':
		case '?':
			print_usage(argv[0], stdout);
			exit(0);
			break;

		case 'i':
			parsec_id = optarg;
			break;
	    
		case 'p':
			if (sscanf(optarg, "%hd", &port) == 1);
			break;

		case 'v':
			verbose++;
			break;

		default:
			print_usage(argv[0], stderr);
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
static void print_usage(char* prog_name, FILE* fout)
{
    fprintf(fout,
	    "Usage: %s [-v] [-d] [-h] [-?] [-i id] [-p port]\n"
	    "    -d     run as a daemon\n"
	    "    -h -?  display this help\n"
	    "    -i     HAB ID (parsec)\n"
	    "    -p     port to receive on (FIXME)\n"
			"    -v     verbose logging\n", prog_name);
} /* print_usage() */
