
// Copyright (c) 2008-2009, Regents of the University of Colorado.
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

#include "bdm-client.h"


extern int bdm_fd;

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
    printf("usage: bdm-client [--server SERVER] ResourceName StartTime EndTime\n");
    printf("       bdm-client --help\n");
    printf("\n");
    printf("    ResourceName is the Resource Name to retrieve.  May contain wildcards.\n");
    printf("\n");
    printf("    StartTime and EndTime are given in this format: \"YYYY-MM-DD hh:mm:ss\"\n");
    printf("        YYYY is the four-digit year, for example 2008\n");
    printf("        MM is the two-digit month, with 01 meaning January and 12 meaning December\n");
    printf("        DD is the two-digit day-of-the-month, with 01 meaning the first day\n");
    printf("        hh:mm:ss is the 24-hour time\n");
    printf("\n");
}




int main(int argc, char *argv[]) {
    char *bdm_hostname = NULL;
    uint16_t bdm_port = BDM_PORT;
    char *resource_name_pattern;
    struct timeval start, stop;

    GPtrArray *hab_list;

    int i;


    for (i = 1; i < argc; i ++) {
        if (strcmp(argv[i], "--server") == 0) {
            i ++;
            bdm_hostname = argv[i];
        } 

        else if (strcmp(argv[i], "--help") == 0) {
            usage();
            exit(0);
        } 

        else {
            // hopefully the required arguments start now
            break;
        }
    }

    if ((argc - i) != 3) {
        usage();
        exit(1);
    }

    resource_name_pattern = argv[i];

    str_to_timeval(argv[i+1], &start);
    str_to_timeval(argv[i+2], &stop);


    bdm_fd = bdm_connect(bdm_hostname, bdm_port);
    if (bdm_fd < 0) {
        exit(1);
    }

    hab_list = bdm_get_resource_datapoints(resource_name_pattern, &start, &stop);
    if (hab_list == NULL) {
        g_message("error getting resource datapoints");
    } else {
        int hi;

        for (hi = 0; hi < hab_list->len; hi ++) {
            bionet_hab_t *hab;
            int ni;

            hab = g_ptr_array_index(hab_list, hi);
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
        }
    }

    bdm_disconnect();

    return 0;
}

