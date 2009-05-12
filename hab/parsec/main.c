
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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
#include <getopt.h>

#include "hardware-abstractor.h"
#include "daemonize.h"

static void parse_cmdline(int argc, char** argv);
static void print_usage(char* prog_name, FILE* fout);
void read_parsec(int fd);
void expire_old_nodes(unsigned int timeout);

static const char* hab_type = "parsec";

static int verbose = 0;
static int daemon_mode = 0;
static unsigned int port = 61557;
static unsigned int timeout = 10;
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
        g_critical("Can't bind to port %hu, quitting./n", port);
        exit(3);	
	}

        
    while(1) {
        fd_set readers;
        int max_fd;
		int ret;
		struct timeval t;

		FD_ZERO(&readers);
		FD_SET(bionet_fd, &readers);
		FD_SET(parsec_fd, &readers);
		max_fd = MAX(bionet_fd, parsec_fd);
		t.tv_sec = timeout;
		t.tv_usec = 0;

		ret = select(max_fd + 1, &readers, NULL, NULL, &t);
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
		expire_old_nodes(timeout);
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
    const char * optstring = "dh?bvi:p:t:s:";
    int c;
    int i;
    char * security_dir = NULL;

    while (1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'b'},
	    {"daemon", 0, 0, 'd'},
	    {"id", 1, 0, 'i'},
	    {"port", 1, 0, 'p'},
	    {"security-dir", 1, 0, 's'},
	    {"timeout", 1, 0, 't'},
	    {"verbose", 0, 0, 'v'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, optstring, long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {
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
	    port = strtoul(optarg, NULL, 0);
	    if (ULONG_MAX == port) {
		g_error("Unable to interpret port: %s - %m\n", optarg);
	    }
	    if (65535 < port) {
		g_error("Invalid port: %u", port);
	    }
	    break;
	    
	case 's':
	    security_dir = optarg;
	    break;

	case 't':
	    if (sscanf(optarg, "%ud", &timeout) != 1) {
		g_error("Unable to interpret timeout: %s\n", optarg);
	    }
	    break;

	case 'v':
	    verbose++;
	    break;

	case 'b':
	    print_bionet_version(stdout);
	    exit(0);
	    
	default:
	    break;
	}
    }

    if (security_dir) {
        if (hab_init_security(security_dir, 1)) {
            g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
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
	    "'%s' PARSEC hardware abstractor\n"
	    "\n"
	    "Usage: %s [-v] [-d] [-?] [-i id] [-p port] [-t sec] [-s dir] [--version]\n"
	    " -d,--daemon               run as a daemon\n"
	    " -?,-h,--help              display this help\n"
	    " -i,--id                   HAB ID (parsec)\n"
	    " -p,--port <port>          port to receive on (FIXME)\n"
	    " -s,--security-dir <dir>   directory containing security certificates\n"
	    " -t,--timeout <sec>        timeout [s] to expire lost nodes\n"
	    " -v,--verbose              verbose logging\n"
	    " --version                 print the version number\n",
	    prog_name, prog_name);
} /* print_usage() */
