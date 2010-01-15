
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <errno.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include <glib.h>

#include "bionet.h"
#include "bdm-client.h"
#include "bionet-util.h"
#include "watcher-callbacks.h"

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

om_t output_mode = OM_NORMAL;
static int bdm_only = 0;

enum subscription_update_t {
    ADD,
    REMOVE,
    RANDOM,
    NONE
};


#if defined(LINUX) || defined(MACOSX)
#include <signal.h>

void signal_handler(int signo) {
    int hi;

    g_message("cache:");

    for (hi = 0; hi < bionet_cache_get_num_habs(); hi ++) {
        int ni;
        bionet_hab_t *hab = bionet_cache_get_hab_by_index(hi);

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

static void str_to_timeval(const char *str, struct timeval *tv) {
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
"'bionet-watcher' displays data from Bionet HABs.\n"
"\n"
"Usage: bionet-watcher [OPTIONS]\n"
"\n"
" --help                                            Show this help\n"
" -e, --require-security                            Require security\n"
" -h, --hab, --habs \"HAB-Type.Hab-ID\"               Subscribe to a HAB list.\n"
" -n, --node, --nodes \"HAB-Type.HAB-ID.Node-ID\"     Subscribe to a Node list\n"
" -r, --resource, --resources \"HAB-Type.HAB-ID.Node-ID:Resource-ID\"\n"
"                                                   Subscribe to Resource values.\n"
" -s, --security-dir <dir>                          Directory containing security\n"
"                                                   certificates\n"
" -a, --add-subscriptions <RATE>                    add new subscriptions every \n"
"                                                   RATE seconds\n"
" -m, --remove-subscriptions <RATE>                 remove subscriptions every\n"
"                                                   RATE seconds\n"
" -d, --randomize-subscriptions <RATE>              randomly add/remove\n"
"                                                   subscriptions every RATE\n"
"                                                   seconds\n"
" -o, --output-mode <MODE>                          either normal (default) or\n"
"                                                   test-pattern for generating\n"
"                                                   test-pattern-hab-input\n"
" -v, --version                                     Show the version number\n"
"\n"
"Security can only be required when a security directory has been specified.\n"
"  bionet-watcher [--security-dir <dir> [--require-security]]\n"
);
}


int main(int argc, char *argv[]) {
    int bionet_fd, bdm_fd;
    char * security_dir = NULL;
    int require_security = 0;
    GSList * hab_list = NULL;
    GSList * node_list = NULL;
    GSList * dp_list = NULL;
    enum subscription_update_t subscription_update = NONE;
    int rate = 0;  // we always set rate before using it, but GCC can't tell
    int urandom_fd = 0, subscribed_to_something = 0;
    g_log_set_default_handler(bionet_glib_log_handler, NULL);
    struct timeval *diff, old;
    struct timeval datapointStart, datapointEnd;
    struct timeval * pDatapointStart = NULL;
    struct timeval * pDatapointEnd = NULL;

    //
    // parse command-line arguments
    //
    int i = 0;
    int c;
    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"hab", 1, 0, 'h'},
	    {"habs", 1, 0, 'h'},
	    {"node", 1, 0, 'n'},
	    {"nodes", 1, 0, 'n'},
	    {"resource", 1, 0, 'r'},
	    {"resources", 1, 0, 'r'},
	    {"security-dir", 1, 0, 's'},
	    {"require-security", 0, 0, 'e'}, 
	    {"add-subscriptions", 1, 0, 'a'},
	    {"remove-subscriptions", 1, 0, 'm'},
	    {"randomize-subscriptions", 1, 0, 'd'},
	    {"output-mode", 1, 0, 'o'},
	    {"datapoint-start", 1, 0, 'T'},
	    {"datapoint-end", 1, 0, 't'},
	    {"bdm-only", 0, 0, 'b'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?vbeh:n:r:s:a:m:d:o:T:t:", long_options, &i);
	if ((-1) == c) {
	    break;
	}

	switch (c) {

	case '?':
	    usage();
	    return 0;

	case 'a':
	    subscription_update = ADD;
        rate = atoi(optarg);
        if (rate < 1) {
            g_message("can't add subscriptions every %d seconds", rate);
            return -1;
        }
	    break;

	case 'b':
	    bdm_only = 1;
	    break;

	case 'd':
	    subscription_update = RANDOM;
        rate = atoi(optarg);
        if (rate < 1) {
            g_message("can't randomize subscriptions every %d seconds", rate);
            return -1;
        }
	    break;

	case 'e':
	    if (security_dir) {
		require_security++;
	    } else {
		usage();
		return (-1);
	    }
	    break;

	case 'h':
	    hab_list = g_slist_append(hab_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'm':
	    subscription_update = REMOVE;
        rate = atoi(optarg);
        if (rate < 1) {
            g_message("can't remove subscriptions every %d seconds", rate);
            return -1;
        }
	    break;

	case 'n':
	    node_list = g_slist_append(node_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'o':
	    if (strcmp(optarg, "normal") == 0) output_mode = OM_NORMAL;
	    else if (strcmp(optarg, "test-pattern") == 0) output_mode = OM_TEST_PATTERN;
	    else {
		fprintf(stderr, "unknown output mode %s\n", optarg);
		usage();
		return 1;
	    }
	    break;
	    
	case 'r':
	    dp_list = g_slist_append(dp_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 's':
	    security_dir = optarg;
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
       
    } //while(1)

    if ((bdm_only > 0) && (pDatapointStart == NULL) && (pDatapointEnd == NULL)) {
	g_log("", G_LOG_LEVEL_ERROR, 
	      "A Datapoint Start and/or End time needs to be specified in BDM-Only mode.");
	exit(1);    
    }

    if ((pDatapointStart || pDatapointEnd) && (subscription_update != NONE)) {
	g_log("", G_LOG_LEVEL_ERROR,
	      "Subscription updates are not available for BDMs, yet...");
	exit(1);
    }

    if (security_dir) {
	if (bionet_init_security(security_dir, require_security)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
	}
    }

    // this must happen before anything else
    if (0 == bdm_only) {
	bionet_fd = bionet_connect();
	if (bionet_fd < 0) {
	    fprintf(stderr, "error connecting to Bionet");
	    exit(1);
	}

	bionet_register_callback_new_hab(cb_new_hab);
	bionet_register_callback_lost_hab(cb_lost_hab);
	
	bionet_register_callback_new_node(cb_new_node);
	bionet_register_callback_lost_node(cb_lost_node);
	
	bionet_register_callback_datapoint(cb_datapoint);
	
	if (subscribed_to_something) {
	    int i;
	    while (g_slist_length(hab_list) > 0) {
		char *hab_sub = g_slist_nth_data(hab_list, 0);
		bionet_subscribe_hab_list_by_name(hab_sub);
		hab_list = g_slist_remove(hab_list, hab_sub);
	    }
	    for (i = 0; i < g_slist_length(node_list); i++) {
		bionet_subscribe_node_list_by_name(g_slist_nth_data(node_list, i));
	    }
	    for (i = 0; i < g_slist_length(dp_list); i++) {
		bionet_subscribe_datapoints_by_name(g_slist_nth_data(dp_list, i));
	    }
	} else {
	    bionet_subscribe_hab_list_by_name("*.*");
	    bionet_subscribe_node_list_by_name("*.*.*");
	    bionet_subscribe_datapoints_by_name("*.*.*:*");
	}
    }

    if (pDatapointStart || pDatapointEnd) {
	bdm_fd = bdm_start();
	if (bdm_fd < 0) {
	    fprintf(stderr, "error connecting to BDM");
	    exit(1);
	}

	bdm_register_callback_new_hab(cb_bdm_new_hab, NULL);
	bdm_register_callback_lost_hab(cb_bdm_lost_hab, NULL);
	
	bdm_register_callback_new_node(cb_bdm_new_node, NULL);
	bdm_register_callback_lost_node(cb_bdm_lost_node, NULL);
	
	bdm_register_callback_datapoint(cb_bdm_datapoint, NULL);

	if (subscribed_to_something) {
	    int i;
	    while (g_slist_length(hab_list) > 0) {
		char *hab_sub = g_slist_nth_data(hab_list, 0);
		bdm_subscribe_hab_list_by_name(hab_sub);
		hab_list = g_slist_remove(hab_list, hab_sub);
	    }
	    for (i = 0; i < g_slist_length(node_list); i++) {
		bdm_subscribe_node_list_by_name(g_slist_nth_data(node_list, i));
	    }
	    for (i = 0; i < g_slist_length(dp_list); i++) {
		bdm_subscribe_datapoints_by_name(g_slist_nth_data(dp_list, i), pDatapointStart, pDatapointEnd);
	    }
	} else {
	    bdm_subscribe_hab_list_by_name("*.*");
	    bdm_subscribe_node_list_by_name("*.*.*");
	    bdm_subscribe_datapoints_by_name("*.*.*:*", pDatapointStart, pDatapointEnd);
	}
    }

    signal(SIGUSR1, signal_handler);

    if (subscription_update != NONE) {
        int r;

        if (subscription_update == RANDOM) {
            urandom_fd = open("/dev/urandom", O_RDONLY);
            if (urandom_fd == -1) {
                g_message("failed to open /dev/urandom: %s", strerror(errno));
                exit(1);
            }
        }

        diff = calloc(1, sizeof(struct timeval));
        if (diff == NULL) {
            g_message("calloc error: %s", strerror(errno));
            exit(1);
        }

        diff->tv_sec = rate;
        diff->tv_usec = 0;

        r = gettimeofday(&old, NULL);
        if (r != 0) {
            g_message("gettimeofday() error: %s", strerror(errno));
            exit(1);
        }

        if (subscription_update == REMOVE) {
            for (i = 0; i < 10; i++) { // start by feeding a giant list of subscriptions
                if (subscribed_to_something) {
                    bionet_subscribe_node_list_by_name(g_slist_nth_data(node_list, 0));
                    bionet_subscribe_datapoints_by_name(g_slist_nth_data(dp_list, 0));
                } else {
                    bionet_subscribe_node_list_by_name("*.*.*");
                    bionet_subscribe_datapoints_by_name("*.*.*:*");
                }
            }
            
            diff->tv_sec = 5;
        }

    } else {
        diff = NULL;
    }

    int max_fd = MAX(bionet_fd, bdm_fd);
    while (1) {

        while (1) {
            int r;
            fd_set readers;

            FD_ZERO(&readers);
            FD_SET(bionet_fd, &readers);
            FD_SET(bdm_fd, &readers);
            r = select(max_fd + 1, &readers, NULL, NULL, diff);

            if ((r < 0) && (errno != EINTR)) {
                fprintf(stderr, "error from select: %s", strerror(errno));
                g_usleep(1000*1000);
                break;
            }

	    if (FD_ISSET(bionet_fd, &readers)) {
		bionet_read();
	    }

	    if (FD_ISSET(bdm_fd, &readers)) {
		bdm_read();
	    }

            if (subscription_update != NONE) {
                struct timeval now;

                r = gettimeofday(&now, NULL);
                if (r != 0) {
                    g_message("gettimeofday() error: %s", strerror(errno));
                    exit(1);
                }

                if (now.tv_sec - old.tv_sec >= 2) {
                    if (subscription_update == ADD) {
                        if (subscribed_to_something) {
                            bionet_subscribe_node_list_by_name(g_slist_nth_data(node_list, 0));
                            bionet_subscribe_datapoints_by_name(g_slist_nth_data(dp_list, 0));
                        } else {
                            bionet_subscribe_node_list_by_name("*.*.*");
                            bionet_subscribe_datapoints_by_name("*.*.*:*");
                        }
                    } else if (subscription_update == REMOVE) {
                        if (subscribed_to_something) {
                            bionet_unsubscribe_node_list_by_name(g_slist_nth_data(node_list, 0));
                            bionet_unsubscribe_datapoints_by_name(g_slist_nth_data(dp_list, 0));
                        } else {
                            printf("removing subscription\n");
                            bionet_unsubscribe_node_list_by_name("*.*.*");
                            bionet_unsubscribe_datapoints_by_name("*.*.*:*");
                        }
                    } else if (subscription_update == RANDOM) {
                        uint8_t rnd;
                        r = read(urandom_fd, &rnd, sizeof(rnd));
                        if (r <= 0) {
                            g_message("bad read: %s", strerror(errno));
                            exit(1);
                        }
                        rnd %= 2;

                        if (rnd == 1) {
                            if (subscribed_to_something) {
                                bionet_subscribe_node_list_by_name(g_slist_nth_data(node_list, 0));
                                bionet_subscribe_datapoints_by_name(g_slist_nth_data(dp_list, 0));
                            } else {
                                bionet_subscribe_node_list_by_name("*.*.*");
                                bionet_subscribe_datapoints_by_name("*.*.*:*");
                            }
                        } else {
                            if (subscribed_to_something) {
                                bionet_unsubscribe_node_list_by_name(g_slist_nth_data(node_list, 0));
                                bionet_unsubscribe_datapoints_by_name(g_slist_nth_data(dp_list, 0));
                            } else {
                                bionet_unsubscribe_node_list_by_name("*.*.*");
                                bionet_subscribe_datapoints_by_name("*.*.*:*");
                            }
                        }
                    }

                    old.tv_sec = now.tv_sec;
                    old.tv_usec = now.tv_usec;

                    diff->tv_sec = rate;
                    diff->tv_usec = 0;
                }
            }
        }
    } 


    // NOT REACHED
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
