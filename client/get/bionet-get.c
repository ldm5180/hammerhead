
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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

#include <glib.h>

#include "bionet.h"
#include "bionet-util.h"


int show_timestamp = 0;


void cb_datapoint(bionet_datapoint_t *datapoint) {
    bionet_value_t *value;
    char *value_str;

    value = bionet_datapoint_get_value(datapoint);
    if (NULL == value) {
        g_warning("Failed to get value from datapoint.");
        exit(1);
    }

    value_str = bionet_value_to_str(value);
    if (value_str == NULL) {
        g_warning("Failed to stringify value.");
        exit(1);
    }

    if (show_timestamp) {
        struct timeval *t = bionet_datapoint_get_timestamp(datapoint);
        g_message(
            "%s %d.%06d",
            value_str,
            (int)t->tv_sec,
            (int)t->tv_usec
        );
    } else {
        g_message("%s", value_str);
    }

    free(value_str);

    exit(0);
}


void usage(void) {
    fprintf(stderr,
        "'bionet-get' prints the current value of a Bionet Resource..\n"
        "\n"
        "Usage: bionet-get [OPTIONS] ResourceName\n"
        "\n"
        " --help                           Show this help\n"
        " -t, --show-timestamp             Output the timestamp of the requested\n"
        "                                  resource (in addition to the value)\n"
        " -e, --require-security           Require security\n"
        " -s, --security-dir <dir>         Directory containing security\n"
        " -v, --version                    Show the version number\n"
        "\n"
        "Security can only be required when a security directory has been specified.\n"
        "  bionet-watcher [--security-dir <dir> [--require-security]]\n"
    );
}


int main(int argc, char *argv[]) {
    int bionet_fd;
    char * security_dir = NULL;
    int require_security = 0;

    struct option long_options[] = {
        {"help", 0, 0, '?'},
        {"show-timestamp", 0, 0, 't'},
        {"version", 0, 0, 'v'},
        {"security-dir", 1, 0, 's'},
        {"require-security", 0, 0, 'e'}, 
        {0, 0, 0, 0} //this must be last in the list
    };

    bionet_log_use_default_handler(NULL);

    //
    // parse command-line arguments
    //
    int i = 0;
    int c;

    while(1) {
        c = getopt_long(argc, argv, "?tves:", long_options, &i);
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

            case 't':
                show_timestamp = 1;
                break;

            case 's':
                security_dir = optarg;
                break;

            case 'v':
                print_bionet_version(stdout);
                return 0;

            default:
                break;
        }
               
    } //while(1)

    if (security_dir) {
        if (bionet_init_security(security_dir, require_security)) {
            g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
        }
    }

    if ((argc - optind) != 1) {
        usage();
        exit(1);
    }

    // this must happen before anything else
    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        fprintf(stderr, "error connecting to Bionet");
        exit(1);
    }

    bionet_register_callback_datapoint(cb_datapoint);
    bionet_subscribe_datapoints_by_name(argv[optind]);


    while (1) {
        bionet_read_with_timeout(NULL);
    } 


    // NOT REACHED
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
