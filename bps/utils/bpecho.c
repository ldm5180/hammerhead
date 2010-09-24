/*
	bpecho.c:	receiver for bundle benchmark test.
									*/
/*									*/
/*	Copyright (c) 2004, California Institute of Technology.		*/
/*	All rights reserved.						*/
/*	Author: Scott Burleigh, Jet Propulsion Laboratory		*/
/*	Enhanced by Ryan Metzger (MITRE Corp.) August 2006		*/
/*	Andrew Jenkins <andrew.jenkins@colorado.edu> made it echo received 
		data, March 2009 			*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "bps/bps_socket.h"

#define ADU_LEN	(1024)

const static int bdlLifetime = 3600;

/*	Indication marks:	"." for BpPayloadPresent (1),
				"*" for BpReceptionTimedOut (2).
 				"!" for BpReceptionInterrupted (3).	*/

static int	running;

static void	handleQuit()
{
	running = 0;
	bps_destroy();
}

#if defined (VXWORKS) || defined (RTEMS)
int	bpecho(int a1, int a2, int a3, int a4, int a5,
		int a6, int a7, int a8, int a9, int a10)
{
	char	*ownEid = (char *) a1;
#else
int	main(int argc, char **argv)
{
	char	*ownEid = (argc > 1 ? argv[1] : NULL);
    char    *ctArg = (argc > 2 ? argv[2] : NULL);
#endif
	char		dataToSend[ADU_LEN];
	ssize_t     bytesToEcho = 0;
    int         custody_requested = 0;

	if (ownEid == NULL)
	{
		puts("Usage: bpecho <own endpoint ID> [ct]");
		return 0;
	}

    if(ctArg && strncmp(ctArg, "ct", 3) == 0) {
        custody_requested = 1;
    }

    int bpfd = bps_socket(0,0,0);
    if ( bpfd < 0 )
	{
		fprintf(stderr, "Can't open bps socket: %s\n", strerror(errno));
		return 1;
	}

    bps_setsockopt(bpfd, SOL_SOCKET, BPS_SO_BDL_LIFETIME, &bdlLifetime, sizeof(bdlLifetime));
    bps_setsockopt(bpfd, SOL_SOCKET, BPS_SO_REQ_CUSTODY, &custody_requested, sizeof(custody_requested));

    struct bps_sockaddr srcaddr;
    strncpy(srcaddr.uri, ownEid, BPS_EID_SIZE);
    if (bps_bind(bpfd, &srcaddr, sizeof(struct bps_sockaddr)))
	{
		fprintf(stderr, "Can't open own endpoint '%s': %s\n", 
            ownEid, strerror(errno));
		return 0;
	}
	
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handleQuit;

	if ( sigaction(SIGINT, &sa, NULL) ) {
        fprintf(stderr, "bpecho failed to set exit handler: %m\n");
    }
	running = 1;
	while (running)
	{
		/*	Wait for a bundle from the driver.		*/
        struct bps_sockaddr remote_addr;
        socklen_t addrlen = sizeof(remote_addr);

        bytesToEcho = bps_recvfrom(bpfd, dataToSend, ADU_LEN, MSG_TRUNC, 
                &remote_addr, &addrlen);

        if(bytesToEcho >= ADU_LEN) {
            fprintf(stderr, "bpecho bundle trucated from %zd to %d bytes\n",
                    bytesToEcho, ADU_LEN);
            bytesToEcho = ADU_LEN;
        }
        if (bytesToEcho < 0 && errno == EINTR) 
        {
            continue;
        }
        if (bytesToEcho <= 0)
        {
            fprintf(stderr, "bpecho bundle reception failed: %m\n");
            bps_close(bpfd);
            return 1;
        }

        putchar('.');
        fflush(stdout);

		/*	Now send acknowledgment bundle.			*/
		if(strcmp(remote_addr.uri, "dtn:none") == 0) continue;

		ssize_t bytes_sent = bps_sendto(bpfd, dataToSend, bytesToEcho, 0,
                &remote_addr, sizeof(struct bps_sockaddr));

		if (bytes_sent < bytesToEcho)
		{
			fprintf(stderr, "bpecho can't send echo bundle\n");
			break;
		}
	}

	bps_close(bpfd);
	return 0;
}
