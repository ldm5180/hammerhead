
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#include "bdm-client.h"


static int str_to_int(const char * str) {
    char * endptr;
    int i;

    i = strtol(str, &endptr, 10);
    if(*endptr != '\0') {
        printf("error parsing int string '%s': Unknown characters '%s'\n", 
            str, endptr);
        exit(1);
    }
    return i;
}

void str_to_timeval(const char *str, struct timeval *tv) {
    struct tm tm;
    char *p;
    char *old_tz;

    p = strptime(str, "%Y-%m-%d %T", &tm);
    if (p == NULL) {
        printf("error parsing time string '%s': %s\n", str, strerror(errno));
        exit(1);
    }

    if (*p == '\0') {
        tv->tv_usec = 0;
    } else {
        int r;
        unsigned int val;
        int consumed;

        if (*p != '.') {
            printf("error parsing fractional seconds from time string '%s': expected decimal point after seconds\n", str);
            exit(1);
        }
        p ++;

        if ((*p < '0') || (*p > '9')) {
            printf("error parsing fractional seconds from time string '%s': expected number after decimal point\n", str);
            exit(1);
        }

        r = sscanf(p, "%6u%n", &val, &consumed);
        // the effect of %n on the returned conversion count is ambiguous
        if ((r != 1) && (r != 2)) {
            printf("error parsing fractional seconds from time string '%s': didn't find \"%%u\"\n", str);
            exit(1);
        }
        if (consumed != strlen(p)) {
            printf("error parsing fractional seconds from time string '%s': garbage at end\n", str);
            exit(1);
        }
        if (consumed > 6) {
            printf("error parsing fractional seconds from time string '%s': number too long\n", str);
            exit(1);
        }

        tv->tv_usec = val * pow(10, 6-consumed);
    }

    old_tz = getenv("TZ");
    setenv("TZ", "UTC", 1);
    tv->tv_sec = mktime(&tm);
    if (old_tz) {
	char sanitized[128];
	snprintf(sanitized, 128, "%s", old_tz);
	setenv("TZ", sanitized, 1);
    } else {
	unsetenv("TZ");
    }
}




void usage(void) {
    fprintf(stderr,
	    "'bdm-client' reads data from the BDM\n"
	    "\n"
	    "usage: bdm-client [OPTIONS]\n"
	    "\n"
	    " -?,--help                          Show this usage information\n"
	    " -v,--version                       Show the version number\n"
	    " -E,--entry-start <entryStart>      Exclude results before this datapoint entry in this\n"
            "                                    local BDM.\n"
	    "                                    integer (default: infinite past)\n"
	    " -e,--entry-end <entryEnd>          Exclude results after this datapoint entry in this\n"
            "                                    local BDM\n"
	    "                                    integer (default: infinite future)\n"
	    " -f,--frequency <seconds>           Query the BDM every N seconds\n"
	    " -p,--port <Port>                   Port number to connect on the server\n"
	    " -r,--resources <Resources>         Resource name pattern of resources to retrieve.\n"
	    "                                    May contain wildcards. (default: \"*.*.*:*\")\n"
	    " -s,--server <server>               BDM server hostname (default: localhost)\n"
	    " -T,--datapoint-start <entryStart>  Timestamp of datapoint as reported by the HAB\n"
	    "                                    time (default: infinite past)\n"
	    " -t,--datapoint-end <entryEnd>      Timestamp of datapoint as reported by the HAB\n"
	    "                                    time (default: infinite future)\n"
	    "\n"
	    "note: StartTime and EndTime are given in this format: \"YYYY-MM-DD hh:mm:ss\"\n"
	    "      YYYY is the four-digit year, for example 2008\n"
	    "      MM is the two-digit month, with 01 meaning January and 12 meaning December\n"
	    "      DD is the two-digit day-of-the-month, with 01 meaning the first day\n"
	    "      hh:mm:ss is the 24-hour time\n"
	    );
}


int main(int argc, char *argv[]) {
    char *bdm_hostname = NULL;
    uint16_t bdm_port = 0;
    char *resource_name_pattern = "*.*.*:*";
    struct timeval datapointStart, datapointEnd;
    int entryStart = -1, entryEnd = -1;
    struct timeval * pDatapointStart = NULL;
    struct timeval * pDatapointEnd = NULL;
    int frequency = 0;

    memset(&datapointStart, 0, sizeof(struct timeval));
    memset(&datapointEnd, 0, sizeof(struct timeval));

    bdm_hab_list_t * hab_list;

    int i;
    int c;

    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"datapoint-start", 1, 0, 'T'},
	    {"datapoint-end", 1, 0, 't'},
	    {"entry-start", 1, 0, 'E'},
	    {"entry-stop", 1, 0, 'e'},
	    {"frequency", 1, 0, 'f'},
	    {"port", 1, 0, 'p'},
	    {"resources", 1, 0, 'r'},
	    {"server", 1, 0, 's'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?hvT:t:E:e:p:r:s:f:", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {
	case '?':
	case 'h':
	    usage();
       	    return 0;

	case 'E':
	    entryStart = str_to_int(optarg);
	    break;

	case 'e':
	    entryEnd = str_to_int(optarg);
	    break;

	case 'f':
	    frequency = str_to_int(optarg);
	    break;

	case 'p':
	    bdm_port = str_to_int(optarg);
	    break;

	case 'r':
	    resource_name_pattern = optarg;
	    break;

	case 's':
	    bdm_hostname = optarg;
	    break;

	case 'T':
	    str_to_timeval(optarg, &datapointStart);
	    pDatapointStart = &datapointStart;
	    break;

	case 't':
	    str_to_timeval(optarg, &datapointEnd);
	    pDatapointEnd = &datapointEnd;
	    break;

	case 'v':
	    print_bionet_version(stdout);
	    return 0;

	default:
	    break;
	}
    }

    if (optind < argc ) {
        printf("Extra unknown arguments:\n");
        for(; optind<argc; optind++ ) printf("   %s\n", argv[optind]);
        printf("\n");
        usage();
        exit(1);
    }


    int fd;
    fd = bdm_connect(bdm_hostname, bdm_port);
    if ( fd < 0 ) {
        exit(1);
    }

restart_poll:
    hab_list = bdm_get_resource_datapoints(resource_name_pattern,
            pDatapointStart, pDatapointEnd, entryStart, entryEnd);
    if (hab_list == NULL) {
        g_message("error getting resource datapoints");
    } else {
        int hi;

        for (hi = 0; hi < bdm_get_hab_list_len(hab_list); hi ++) {
            bionet_hab_t *hab;
            int ni;

            hab = bdm_get_hab_by_index(hab_list, hi);
            printf("%s.%s\n", bionet_hab_get_type(hab), bionet_hab_get_id(hab));

            for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
                bionet_node_t *node;
                int ri;

                node = bionet_hab_get_node_by_index(hab, ni);
                printf("    %s\n", bionet_node_get_id(node));

                for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
                    bionet_resource_t *resource;
                    int di;

                    resource = bionet_node_get_resource_by_index(node, ri);
                    printf(
                        "        %s %s %s\n",
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        bionet_resource_get_id(resource)
                    );

                    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
                        bionet_datapoint_t *d;

                        d = bionet_resource_get_datapoint_by_index(resource, di);

                        printf(
                            "            %s @ %s\n",
                            bionet_value_to_str(bionet_datapoint_get_value(d)),
                            bionet_datapoint_timestamp_to_string(d)
                        );
                    }
                }
            }
	    bionet_hab_free(hab);
        }
    }

    if (frequency) {
	sleep(frequency);
	if ((hab_list != NULL) && (bdm_get_hab_list_last_entry_seq(hab_list))) {
	    entryStart = bdm_get_hab_list_last_entry_seq(hab_list) + 1;
	    bdm_hab_list_free(hab_list);
	}
	goto restart_poll;
    }

    bdm_disconnect();

    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
