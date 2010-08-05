
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
#include <glib.h>

#include "bdm-client.h"
   
extern void default_output_register_callbacks(void);
extern void test_pattern_output_register_callbacks(void);
extern void bdm_client_output_register_callbacks(void);
extern void bdm_watcher_output_register_callbacks(void);

static GMainLoop *bdmsub_main_loop = NULL;

typedef enum {
    OM_NORMAL,
    OM_TEST_PATTERN,
    OM_BDM_CLIENT,
    OM_BDM_WATCHER,
} om_t;

om_t output_mode = OM_NORMAL;


enum subscription_update_t {
    ADD,
    REMOVE,
    RANDOM,
    NONE
};


#if defined(LINUX) || defined(MACOSX)
#include <signal.h>

void stop_signal_handler(int signo) {
    g_main_loop_quit(bdmsub_main_loop);
}

void signal_handler(int signo) {
    int hi;

    g_message("cache:");

    for (hi = 0; hi < bdm_cache_get_num_habs(); hi ++) {
        int ni;
        bionet_hab_t *hab = bdm_cache_get_hab_by_index(hi);

        g_message("    %s", bionet_hab_get_name(hab));

        for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
            int i;
            bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);

            g_message("        %s", bionet_node_get_id(node));

            for (i = 0; i < bionet_node_get_num_resources(node); i ++) {
                bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
                bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, 0);
		bionet_value_t *value = bionet_datapoint_get_value(d);
                if (d == NULL) {
                    g_message(
                        "            %s (%s %s): (no value)",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
                    );
                } else {
		    char * value_str = bionet_value_to_str(value);
                    g_message(
                        "            %s (%s %s):  %s @ %s",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        value_str,
                        bionet_datapoint_timestamp_to_string(d)
                    );
		    free(value_str);
                }
            }
            for (i = 0; i < bionet_node_get_num_streams(node); i ++) {
                bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
                g_message(
                    "            %s %s %s", 
                    bionet_stream_get_id(stream),
                    bionet_stream_get_type(stream),
                    bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
                );
            }
        }
    }
}

#endif /* defined(LINUX) || defined(MACOSX) */


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

    memset(&tm, 0, sizeof(tm));
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
	    "'bdm-watcher' displays data from the Bionet Data Managers \n"
	    "\n"
	    "usage: bdm-watcher [OPTIONS]\n"
	    "\n"
	    " -?,--help                          Show this usage information\n"
	    " -v,--version                       Show the version number\n"
            " -b, --bdm, --bdms <pattern>        Subscribe to a BDM list.\n"
            "                                    \"BDM-ID,Recording-BDM-ID\" \n"
            " -h, --hab, --habs <pattern>        Subscribe to a HAB list.\n"
            "                                    \"[[BDM-ID,]Recording-BDM-ID/]HAB-Type.Hab-ID\" \n"
            " -n, --node, --nodes <pattern>      Subscribe to a Node list\n"
            "                                    \"[[BDM-ID,]Recording-BDM-ID/]HAB-Type.HAB-ID.Node-ID\"\n"
	    " -r,--resources,--resources <pattern> \n"
            "                                    Subscribe to updates to this resource name pattern\n"
            "                                    \"[[BDM-ID,]Recording-BDM-ID/]HAB-Type.HAB-ID.Node-ID:Resource-ID\"\n"
	    "                                    May contain wildcards. (default: \"*,*/*.*.*:*\")\n"
	    " -s,--server <server:port>          Also subscribe to updates from this BDM\n"
            "                                    Needed only if the BDM is not on the local link\n"
	    " -T,--datapoint-start <start-time>  Timestamp of datapoint as reported by the HAB\n"
	    "                                    time (default: Current time)\n"
	    " -t,--datapoint-end <end-time>      Timestamp of datapoint as reported by the HAB\n"
	    "                                    time (default: infinite future)\n"
            " -0,--output-mode <MODE>            Format the output. Can be one of:\n"
            "                                     normal       (default)\n"
            "                                     test-pattern (For generating test-pattern-hab input)\n"
            "                                     bdm-client   (Sortable, comma separated)\n"
            "                                     bdm-watcher  (like bdm-client, but with recording bdm)\n"
	    "note: StartTime and EndTime are given in this format: \"YYYY-MM-DD hh:mm:ss\"\n"
	    "      YYYY is the four-digit year, for example 2008\n"
	    "      MM is the two-digit month, with 01 meaning January and 12 meaning December\n"
	    "      DD is the two-digit day-of-the-month, with 01 meaning the first day\n"
	    "      hh:mm:ss is the 24-hour time\n"
	    );
}

//
// Whenever the bdm handle is readable, call bdm_read()
//
static int bdm_readable_handler(GIOChannel *listening_ch,
        GIOCondition condition,
        gpointer usr_data) 
{
    bdm_read();

    return TRUE;
}
        

int main(int argc, char *argv[]) {
    char *bdm_hostname = NULL;
    uint16_t bdm_port = 0 ;
    int subscribed_to_something = 0;
    struct timeval datapointStart, datapointEnd;
    struct timeval * pDatapointStart = NULL;
    struct timeval * pDatapointEnd = NULL;
    GSList * bdm_list = NULL;
    GSList * hab_list = NULL;
    GSList * node_list = NULL;
    GSList * dp_list = NULL;

    memset(&datapointStart, 0, sizeof(struct timeval));
    memset(&datapointEnd, 0, sizeof(struct timeval));

    bionet_log_use_default_handler(NULL);

    int i;
    int c;

    if ( 0 == gettimeofday(&datapointStart, NULL) ) {
        pDatapointStart = &datapointStart;
    }

    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"bdm", 1, 0, 'b'},
	    {"bdms", 1, 0, 'b'},
	    {"hab", 1, 0, 'h'},
	    {"habs", 1, 0, 'h'},
	    {"node", 1, 0, 'n'},
	    {"nodes", 1, 0, 'n'},
	    {"resource", 1, 0, 'r'},
	    {"resources", 1, 0, 'r'},
	    {"datapoint-start", 1, 0, 'T'},
	    {"datapoint-end", 1, 0, 't'},
	    {"port", 1, 0, 'p'},
	    {"server", 1, 0, 's'},
	    {"output-mode", 1, 0, 'o'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?h:vn:o:p:r:s:T:t:", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {
	case '?':
	    usage();
       	    return 0;

	case 'b':
	    bdm_list = g_slist_append(bdm_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'h':
	    hab_list = g_slist_append(hab_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'n':
	    node_list = g_slist_append(node_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'r':
	    dp_list = g_slist_append(dp_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 's': {
	    bdm_hostname = strdup(optarg);
            char * p = strchr(bdm_hostname, ':'); 
            if ( NULL == p ) {
                fprintf(stderr, "%s not <hostname>:<port>\n", optarg);
                return 1;
            }
            p[0] = '\0';
            p++;
            bdm_port = str_to_int(p);
	    break;
        }

	case 'T':
	    str_to_timeval(optarg, &datapointStart);
	    pDatapointStart = &datapointStart;
	    break;

	case 't':
	    str_to_timeval(optarg, &datapointEnd);
	    pDatapointEnd = &datapointEnd;
	    break;

        case 'o':
            if (strcmp(optarg, "normal") == 0) output_mode = OM_NORMAL;
            else if (strcmp(optarg, "test-pattern") == 0) output_mode = OM_TEST_PATTERN;
            else if (strcmp(optarg, "bdm-client") == 0) output_mode = OM_BDM_CLIENT;
            else if (strcmp(optarg, "bdm-watcher") == 0) output_mode = OM_BDM_WATCHER;
            else {
                fprintf(stderr, "unknown output mode %s\n", optarg);
                usage();
                return 1;
            }
            break;

	case 'v':
	    print_bionet_version(stdout);
	    return 0;

	default:
	    break;
	}
    }

    bdmsub_main_loop = g_main_loop_new(NULL, TRUE);

    {
        GIOChannel *ch;
        int fd = bdm_start();
        if (fd < 0) {
            exit(1);
        }

        ch = g_io_channel_unix_new(fd);
        g_io_add_watch(ch, G_IO_IN, bdm_readable_handler, NULL);

    }


    // register callbacks
    if ( output_mode == OM_NORMAL ) {
        default_output_register_callbacks();
    } else if ( output_mode == OM_TEST_PATTERN ) {
        test_pattern_output_register_callbacks();
    } else if ( output_mode == OM_BDM_CLIENT ) {
        bdm_client_output_register_callbacks();
    } else if ( output_mode == OM_BDM_WATCHER ) {
        bdm_watcher_output_register_callbacks();
    }

    // Add subscriptions
    if (! subscribed_to_something) {
        bdm_list = g_slist_append(bdm_list, "*,*");
        hab_list = g_slist_append(hab_list, "*,*/*.*");
        node_list = g_slist_append(node_list, "*,*/*.*.*");
        dp_list = g_slist_append(dp_list, "*,*/*.*.*:*");
    }
    for (i = 0; i < g_slist_length(bdm_list); i++) {
        bdm_subscribe_bdm_list_by_name(g_slist_nth_data(bdm_list, i));
    }
    for (i = 0; i < g_slist_length(hab_list); i++) {
        bdm_subscribe_hab_list_by_name(g_slist_nth_data(hab_list, i));
    }
    for (i = 0; i < g_slist_length(node_list); i++) {
        bdm_subscribe_node_list_by_name(g_slist_nth_data(node_list, i));
    }
    for (i = 0; i < g_slist_length(dp_list); i++) {
        const char * dp_name = g_slist_nth_data(dp_list, i);

        bdm_subscribe_datapoints_by_name(dp_name, pDatapointStart, pDatapointEnd);
    }

    g_slist_free(bdm_list);
    g_slist_free(hab_list);
    g_slist_free(node_list);
    g_slist_free(dp_list);

    signal(SIGUSR1, signal_handler);
    signal(SIGTERM, stop_signal_handler);
    signal(SIGINT, stop_signal_handler);

    g_main_loop_run(bdmsub_main_loop);

    bdm_disconnect();

    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
