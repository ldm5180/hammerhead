/*
 * bping.c <andrew.jenkins@colorado.edu>
 * Performs an ICMP-echo-like "ping" over BP, for ION.
 * This program attaches to the source endpoint, then sends a bundle every
 * INTERVAL seconds to the destination endpoint.  The contents of the bundle
 * are a unique identifier.  If the destination endpoint echos these bundles
 * back (for example, via bpecho), the round-trip time is displayed.
 */

#include <stdio.h> 
#include <string.h>     /* strtok_r() */
#include <getopt.h>     /* getopt */
#include <lyst.h>
#include <limits.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>

#include "bps/bps_socket.h"

const char usage[] =
  "Usage: bping [options] <source EID> <destination EID> [report-to EID]\n" 
  "             [smkey]\n\n"
  "Sends bundles from <source EID> to <destination EID>.  If responses are\n"
  "received, prints the elapsed round trip time.\n"
  "Options:\n"
  "  -c <count>     Sends <count> bundles before stopping.\n"
  "  -i <interval>  Wait <interval> seconds between bundles.  Default: 1.\n"
  "  -p <priority>  Bundles have priority <priority> (default 0 = bulk).\n"
  "  -q <wait>      Wait <wait> seconds after sending the last bundle to\n"
  "                   accumulate responses.  Defaults to 10s, pass -1 to\n"
  "                   wait until all bundles are acked before quitting.\n"
#if 0
  "  -r <flags>     <flags> can be any combination of rcv,ct,fwd,dlv,del,ctr\n"
  "                   delimited by ',' (without spaces).  Sets the \n"
  "                   corresponding report request flag.\n"
#endif
  "  -t <ttl>       Bundles have lifetime of <ttl> seconds.\n"
  "                   (default 3600)\n"
  "  -v             Increase verbosity level (can be specified repeatedly)\n";

static int count = -1;        /* -1: Indefinite.  Set from command line, 
                                 never written again. */
static int interval = 1;      /* Wait one second between bundles */
static int verbosity = 0;
static int waitdelay = 10; /* Number of seconds to wait after last 
                                 bundle for its response. */
static int ttl = 3600;        /* Lifetime to set in bundles. */
static int priority = 0;      /* Priority level of bundles. */
//static int rrFlags = 0;       /* Report request flags */
static int totalsent = 0;     /* Only written by sendRequests thread */
static int totalreceived = 0; /* Only written by receiveResponses thread */
static int shutdownnow = 0;      /* 1: Cleanup and shutdown. */
static int custodySwitch = 0;

static char     *srcEid, *dstEid, *rptEid;

#define BPING_PAYLOAD_MAX_LEN 256

/* These exit codes are the same as iputils' ping. */
#define BPING_EXIT_SUCCESS              (0)
#define BPING_EXIT_NOTALLRESPONDED      (1)
#define BPING_EXIT_ERROR                (2)

/* All values are kept in us (us^2 for sum2), just like iputils. */
static long min = LONG_MAX, max = 0, dev;
static long long sum = 0, sum2 = 0;

/* iputils uses Newton's method to ping; we copy that here.  The method below 
 * will find the largest integer less than or equal to the square root of a, 
 * unless (a+1) is a perfect square, in which case it will return sqrt(a+1).
 * From iputils' ping_common.c */
static long llsqrt(long long a)
{
	long long prev = ~((long long)1 << 63);
	long long x = a;

	if (x > 0) {
		while (x < prev) {
			prev = x;
			x = (x+(a/x))/2;
		}
	}

	return (long)x;
}

static void handleQuit()
{
	shutdownnow = 1;
}

/* Subtract the `struct timeval' values X and Y, storing the result in RESULT.
 * Return 1 if the difference is negative, otherwise 0.
 * From GNU libc reference manual sec. 21.2. */
static int
timeval_subtract (result, x, y)
     struct timeval *result, *x, *y;
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}


int handleResponse(int fd)
{
	struct timeval tvNow, tvResp, tvDiff;
	size_t      contentLength;
	char        buffer[BPING_PAYLOAD_MAX_LEN];
	char        *saveptr, *countstr, *secstr, *usecstr, *endptr;
	unsigned long respcount;
	long        diff_in_us;

	struct bps_sockaddr addr;
	socklen_t addrlen = sizeof(struct bps_sockaddr);



	contentLength = bps_recvfrom(fd, buffer, sizeof(buffer)-1, 0,
			&addr, &addrlen);
	if(contentLength <= 0 ){
		return -1;
	}
	buffer[contentLength] = '\0';

	/* Get the time the response was received */
	if(gettimeofday(&tvNow, NULL) < 0) {
		perror("Couldn't gettimeofday");
		return -1;
	}


	/* If this bundle isn't from the right source, ignore. */
	if(strcmp(addr.uri, dstEid) != 0) {
		if(verbosity) {
			fprintf(stderr, "Ignoring a bundle from %s\n", addr.uri);
		}
		return 0;
	}

	/* Parse out the values in the response */
	countstr = strtok_r(buffer, " ", &saveptr);
	if(countstr == NULL) {
		fprintf(stderr, "Couldn't parse countstr.\n");
		return -1;
	}
	secstr = strtok_r(NULL, " ", &saveptr);
	if(secstr == NULL) {
		fprintf(stderr, "Couldn't parse secstr.\n");
		return -1;
	}
	usecstr = strtok_r(NULL, " ", &saveptr);
	if(usecstr == NULL) {
		fprintf(stderr, "Couldn't parse usecstr.\n");
		return -1;
	}

	respcount = strtoul(countstr, &endptr, 0);
	if(endptr == NULL) {
		fprintf(stderr, "Couldn't convert countstr: %s\n", countstr);
		return -1;
	}
	tvResp.tv_sec = strtoul(secstr, &endptr, 0);
	if(endptr == NULL) {
		fprintf(stderr, "Couldn't convert secstr: %s\n", secstr);
		return -1;
	}
	tvResp.tv_usec = strtoul(usecstr, &endptr, 0);
	if(endptr == NULL) {
		fprintf(stderr, "Couldn't convert usecstr: %s\n", usecstr);
		return -1;
	}

	/* Print the result */
	if(timeval_subtract(&tvDiff, &tvNow, &tvResp) == 1) {
		/* Diff is negative */
		diff_in_us = - (tvDiff.tv_sec * 1000000 + tvDiff.tv_usec);
	} else {
		/* Diff is positive */
		diff_in_us =   (tvDiff.tv_sec * 1000000 + tvDiff.tv_usec);
	}

	if(diff_in_us < 0) {
		printf("%zd bytes from %s  seq=%lu time=-%lu.%06lu s(future!)\n",
				contentLength, addr.uri, respcount, tvDiff.tv_sec, 
				(unsigned long)tvDiff.tv_usec);
	} else {
		printf("%zd bytes from %s  seq=%lu time=%lu.%06lu s\n",
				contentLength, addr.uri, respcount, tvDiff.tv_sec, 
				(unsigned long)tvDiff.tv_usec);
	}

	/* Update statistics */
	if(diff_in_us < min) min = diff_in_us;
	if(diff_in_us > max) max = diff_in_us;
	sum += diff_in_us;
	sum2 += (long long)(diff_in_us) * (long long)(diff_in_us);

	++totalreceived; /* Successful receipt */

	return 0;
}

/* Makes a new bundle Object.  The payload is a unique identifier. */
static int bping_send_ping(int fd)
{
	struct timeval tvNow;
	char    pingPayload[BPING_PAYLOAD_MAX_LEN];
	int     pingPayloadLen;


	if(gettimeofday(&tvNow, NULL) < 0) {
		perror("bping_new_ping gettimeofday()");
		return -1;
	}

	/* Construct the bundle payload */
	pingPayloadLen = snprintf(pingPayload, sizeof(pingPayload), 
			"%d %lu %lu bping payload", totalsent, tvNow.tv_sec, 
			(unsigned long)tvNow.tv_usec);
	if(pingPayloadLen < 0) {
		fprintf(stderr, "Couldn't construct bping payload.");
		return -1;
	}

    if(pingPayloadLen >= sizeof(pingPayload)) {
        pingPayloadLen = sizeof(pingPayload) - 1;
    }

	ssize_t bytes = bps_send(fd, pingPayload, pingPayloadLen, 0);
	if(bytes < 0 ) {
        perror("Error sending bping");
		return -1;
	}
	if(bytes != pingPayloadLen){
		fprintf(stderr, "Error sending bping: Short write\n");
		return -1;
	}
	++totalsent;    /* Successful send */

	return 0;
}

#if 0
static void parse_report_flags(int *srrFlags, const char *flags) {
	char myflags[1024];
	char *saveptr, *token;

	/* make a local copy of flags */
	strncpy(myflags, flags, 1023);
	myflags[1023] = '\0';

	/* parse flags */
	token = strtok_r(myflags, ",", &saveptr);
	while(token != NULL) {
		if (strcmp(token, "rcv") == 0)
			(*srrFlags) |= BP_RECEIVED_RPT;
		if (strcmp(token, "ct") == 0)
			(*srrFlags) |= BP_CUSTODY_RPT;
		if (strcmp(token, "fwd") == 0)
			(*srrFlags) |= BP_FORWARDED_RPT;
		if (strcmp(token, "dlv") == 0)
			(*srrFlags) |= BP_DELIVERED_RPT;
		if (strcmp(token, "del") == 0)
			(*srrFlags) |= BP_DELETED_RPT;
		if (strcmp(token, "ctr") == 0)
			custodySwitch = SourceCustodyRequired;

		token = strtok_r(NULL, ",", &saveptr);
	}
}
#endif

int main(int argc, char **argv)
{
	int ch;
	struct timeval tvStart, tvStop, tvDiff, tvNextPing, tvLastPing;

	if(gettimeofday(&tvStart, NULL) < 0) {
		perror("Couldn't get start time");
		exit(BPING_EXIT_ERROR);
	}

	while ((ch = getopt(argc, argv, "+c:i:hp:q:r:s:t:v")) != EOF) {
		switch(ch) {
			case 'c':
				count = atoi(optarg);
				break;
			case 'i':
				interval = atoi(optarg);
				break;
			case 'h':
				fprintf(stderr, "%s", usage);
				exit(BPING_EXIT_ERROR);
				break;
			case 'p':
				priority = atoi(optarg);
				break;
			case 'q':
				waitdelay = atoi(optarg);
				break;
#if 0
			case 'r':
				parse_report_flags(&rrFlags, optarg);
				break;
#endif
			case 't':
				ttl = atoi(optarg);
				break;
			case 'v':
				verbosity++;
				break;
			default:
				fprintf(stderr, "Couldn't handle option %c (%02x)\n", ch, ch);
				exit(BPING_EXIT_ERROR);
		}
	}

	if(argc - optind < 2) {
		fprintf(stderr, "%s", usage);
		exit(BPING_EXIT_ERROR);
	}

	srcEid = argv[optind];
	dstEid = argv[optind + 1];
	rptEid = (argc - optind > 2) ? argv[optind + 2] : NULL;

	if(verbosity) {
		fprintf(stderr, "Sending %d bundles from %s to %s (rpt-to: %s) "
				"every %d seconds\n", count, srcEid, dstEid, 
				rptEid ? rptEid : "none", interval);
	}

	int bpfd = bps_socket(0,0,0);
	if ( bpfd < 0 ) {
		perror("Can't create bps socket: %s.\n");
		exit(BPING_EXIT_ERROR);
	}

    bps_setsockopt(bpfd, SOL_SOCKET, BPS_SO_BDL_LIFETIME, &ttl, sizeof(ttl));
    bps_setsockopt(bpfd, SOL_SOCKET, BPS_SO_BDL_PRIORITY, &priority, sizeof(priority));
    bps_setsockopt(bpfd, SOL_SOCKET, BPS_SO_REQ_CUSTODY, &custodySwitch, sizeof(custodySwitch));

	struct bps_sockaddr srcaddr;
	strncpy(srcaddr.uri, srcEid, BPS_EID_SIZE);
	if (bps_bind(bpfd, &srcaddr, sizeof(struct bps_sockaddr)))
	{
		fprintf(stderr, "Can't bind bps socket to %s: %s.\n", 
					   srcaddr.uri, strerror(errno));
		exit(BPING_EXIT_ERROR);
	}

	struct bps_sockaddr dstaddr;
	strncpy(dstaddr.uri, dstEid, BPS_EID_SIZE);
	if (bps_connect(bpfd, &dstaddr, sizeof(struct bps_sockaddr)))
	{
		fprintf(stderr, "Can't connect bps socket to %s: %s.\n", 
					   dstaddr.uri, strerror(errno));
		exit(BPING_EXIT_ERROR);
	}

	signal(SIGINT, handleQuit);

	tvNextPing = tvStart;
    tvLastPing.tv_sec = 0;
    tvLastPing.tv_usec = 0;
	while((shutdownnow == 0) 
	&& (count == -1 
		|| totalsent < count 
		|| waitdelay 
	   )
	) {
		struct timeval tvDelay, tvNow;

		fd_set readers;
		FD_ZERO(&readers);
		FD_SET(bpfd, &readers);

		// Calculate how long to sleep
		if(gettimeofday(&tvNow, NULL) < 0) {
			perror("bping_new_ping gettimeofday()");
			return -1;
		}

		if( count > 0 && totalsent >= count ) {
			// Receiving bundles until timeout...
			timeval_subtract(&tvDelay, &tvNow, &tvLastPing);
			if(totalreceived == totalsent || tvDelay.tv_sec >= waitdelay) {
				break;
			}
		} else if(timeval_subtract(&tvDelay, &tvNextPing, &tvNow) == 1) {
			/* Ping timeout has expired */
			tvLastPing = tvNow;

			bping_send_ping(bpfd);

			tvNextPing.tv_sec = tvNow.tv_sec + interval;
			tvNextPing.tv_usec = tvNow.tv_usec;

			tvDelay.tv_sec = interval;
			tvDelay.tv_usec = 0;
		}

        int r = select(bpfd+1, &readers, NULL, NULL, &tvDelay);
		if ( r < 0 ) {
			if ( errno == EINTR ) {
				continue;
			}
			perror("select() error: %s.\n");
		} else if ( r ) {
			// Parse response
			handleResponse(bpfd);
		}
	}
	if(verbosity) fprintf(stderr, "Sent %d bundles.\n", totalsent);


	bps_close(bpfd);

	/* Calculate statistics. */
	if(gettimeofday(&tvStop, NULL) < 0) {
		perror("Couldn't get stop time");
		exit(BPING_EXIT_ERROR);
	}
	if(timeval_subtract(&tvDiff, &tvStop, &tvStart) < 0) {
		fprintf(stderr, "Problem subtracting timevals\n");
		exit(BPING_EXIT_ERROR);
	}

	printf("%d bundles transmitted, %d bundles received, %.2f%% bundle"
			" loss, time %lu.%06lu s\n", totalsent, totalreceived,
			100.0*(1 - ((double)totalreceived)/((double)totalsent)),
			tvDiff.tv_sec, (unsigned long)tvDiff.tv_usec);

	if(totalreceived > 0) {
		sum  /= totalreceived;
		sum2 /= totalreceived;
		dev   = llsqrt(sum2 - sum * sum);

		printf("rtt min/avg/max/sdev = "
				"%ld.%03lu/%ld.%03lu/%ld.%03lu/%ld.%03ld ms\n",
				min/1000L,          (unsigned long)(min)%1000UL, 
				(long)(sum/1000LL), (unsigned long)(sum)%1000UL,
				max/1000L,          (unsigned long)(max)%1000UL,
				dev/1000L,          (unsigned long)(dev)%1000UL);
	}

	if(totalreceived == totalsent) return BPING_EXIT_SUCCESS;
	return BPING_EXIT_NOTALLRESPONDED;
}
