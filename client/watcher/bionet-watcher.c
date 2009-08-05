
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
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
#include "bionet-util.h"

typedef enum {
    OM_NORMAL,
    OM_TEST_PATTERN
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


static const char *current_timestamp_string(void)
{
    static char time_str[200];

    char usec_str[10];
    struct tm *tm;
    int r;

    // 
    // sanity tests
    //
    struct timeval timestamp;
    gettimeofday(&timestamp, NULL);

    tm = gmtime(&timestamp.tv_sec);
    if (tm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): error with gmtime: %s", 
	      strerror(errno));
        return "invalid time";
    }

    r = strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
    if (r <= 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): error with strftime: %s", 
	      strerror(errno));
        return "invalid time";
    }

    r = snprintf(usec_str, sizeof(usec_str), ".%06ld", (long)timestamp.tv_usec);
    if (r >= sizeof(usec_str)) {
        // this should never happen, but it keeps Coverity happy
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): usec_str too small?!");
        return "invalid time";
    }

    // sanity check destination memory size available
    if ((strlen(usec_str) + 1 + strlen(time_str)) > sizeof(time_str)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): time_str too small?!");
        return "invalid time";
    }
    strncat(time_str, usec_str, strlen(usec_str));

    return time_str;
}


void cb_datapoint(bionet_datapoint_t *datapoint) {
    bionet_value_t * value = bionet_datapoint_get_value(datapoint);
    if (NULL == value) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to get value from datapoint.");
	return;
    }

    bionet_resource_t * resource = bionet_value_get_resource(value);
    if (NULL == resource) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource from value.");
	return;
    }

    char * value_str = bionet_value_to_str(value);

    if (output_mode == OM_NORMAL) {
        g_message(
            "%s = %s %s %s @ %s",
            bionet_resource_get_name(resource),
            bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
            bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
            value_str,
            bionet_datapoint_timestamp_to_string(datapoint)
        );
    } else if (output_mode == OM_TEST_PATTERN) {
        g_message(
            "%s %s %s '%s'",
            bionet_datapoint_timestamp_to_string(datapoint),
            bionet_node_get_id(bionet_resource_get_node(resource)),
            bionet_resource_get_id(resource),
            value_str
        );
    }

    free(value_str);
}


void cb_lost_node(bionet_node_t *node) {
    if (output_mode == OM_NORMAL) {
        g_message("lost node: %s", bionet_node_get_name(node));
    } else if (output_mode == OM_TEST_PATTERN) {
        g_message("%s - %s", 
            current_timestamp_string(),
            bionet_node_get_id(node));
    }
}


void cb_new_node(bionet_node_t *node) {
    int i;

    if (output_mode == OM_NORMAL) {
        g_message("new node: %s", bionet_node_get_name(node));
    } else if (output_mode == OM_TEST_PATTERN) {
        g_message("%s + %s", 
            current_timestamp_string(),
            bionet_node_get_id(node));
    }

    if (bionet_node_get_num_resources(node)) {
        if (output_mode == OM_NORMAL)
            g_message("    Resources:");

        for (i = 0; i < bionet_node_get_num_resources(node); i++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
	    if (NULL == resource) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource at index %d from node", i);
		continue;
	    }
            bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);

            if (datapoint == NULL) {
                if (output_mode == OM_NORMAL) {
                    g_message(
                        "        %s %s %s (no known value)", 
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        bionet_resource_get_id(resource)
                    );
                } else if (output_mode == OM_TEST_PATTERN) {
                    g_message(
                        "    %s %s %s ?",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
                    );
                }
            } else {
                char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

                if (output_mode == OM_NORMAL) {
                    g_message(
                        "        %s %s %s = %s @ %s", 
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        bionet_resource_get_id(resource),
                        value_str,
                        bionet_datapoint_timestamp_to_string(datapoint)
                    );
                } else if (output_mode == OM_TEST_PATTERN) {
                    g_message(
                        "    %s %s %s '%s'",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        value_str
                    );
                }

		free(value_str);
            }

        }
    }

    if (bionet_node_get_num_streams(node)) {
        if (output_mode == OM_NORMAL)
            g_message("    Streams:");

        for (i = 0; i < bionet_node_get_num_streams(node); i++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            if (NULL == stream) {
                g_log("", G_LOG_LEVEL_WARNING, "Failed to get stream at index %d from node", i);
            }

            if (output_mode == OM_NORMAL) {
                g_message(
                    "        %s %s %s", 
                    bionet_stream_get_id(stream),
                    bionet_stream_get_type(stream),
                    bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
                );
            }
        }
    }
}


void cb_lost_hab(bionet_hab_t *hab) {
    if (output_mode == OM_NORMAL) {
        g_message("lost hab: %s", bionet_hab_get_name(hab));
    }
}


void cb_new_hab(bionet_hab_t *hab) {
    if (output_mode == OM_NORMAL) {
        g_message("new hab: %s", bionet_hab_get_name(hab));
        if (bionet_hab_is_secure(hab)) {
            g_message("    %s: security enabled", bionet_hab_get_name(hab));
        }
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
    int bionet_fd;
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
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?veh:n:r:s:a:m:d:o:", long_options, &i);
	if ((-1) == c) {
	    break;
	}

	switch (c) {

	case '?':
	    usage();
	    return 0;

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

	case 'n':
	    node_list = g_slist_append(node_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'r':
	    dp_list = g_slist_append(dp_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'a':
	    subscription_update = ADD;
        rate = atoi(optarg);
        if (rate < 1) {
            g_message("can't add subscriptions every %d seconds", rate);
            return -1;
        }
	    break;

	case 'm':
	    subscription_update = REMOVE;
        rate = atoi(optarg);
        if (rate < 1) {
            g_message("can't remove subscriptions every %d seconds", rate);
            return -1;
        }
	    break;

	case 'd':
	    subscription_update = RANDOM;
        rate = atoi(optarg);
        if (rate < 1) {
            g_message("can't randomize subscriptions every %d seconds", rate);
            return -1;
        }
	    break;

	case 's':
	    security_dir = optarg;
	    break;
	    
	case 'v':
	    print_bionet_version(stdout);
	    return 0;

    case 'o':
        if (strcmp(optarg, "normal") == 0) output_mode = OM_NORMAL;
        else if (strcmp(optarg, "test-pattern") == 0) output_mode = OM_TEST_PATTERN;
        else {
            fprintf(stderr, "unknown output mode %s\n", optarg);
            usage();
            return 1;
        }
        break;

	default:
	    break;
	}
       
    } //while(1)

    if (security_dir) {
	if (bionet_init_security(security_dir, require_security)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
	}
    }

    // this must happen before anything else
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

    while (1) {

        while (1) {
            int r;
            fd_set readers;

            FD_ZERO(&readers);
            FD_SET(bionet_fd, &readers);

            r = select(bionet_fd + 1, &readers, NULL, NULL, diff);

            if ((r < 0) && (errno != EINTR)) {
                fprintf(stderr, "error from select: %s", strerror(errno));
                g_usleep(1000*1000);
                break;
            }

            bionet_read();

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
