
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
    r = gettimeofday(&timestamp, NULL);
    if (r != 0) {
        g_message("gettimeofday() error: %s", strerror(errno));
        exit(1);
    }


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


void cbbc_datapoint(bionet_datapoint_t *datapoint, void * usr_data) {
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

    bionet_node_t * node = bionet_resource_get_node(resource);
    if (NULL == node) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to get node from resource.");
	return;
    }


    bionet_hab_t * hab = bionet_node_get_hab(node);
    if (NULL == hab) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to get hab from node.");
	return;
    }

    char * value_str = bionet_value_to_str(value);

    g_message(
        "%s,%s.%s:%s,%s",
        bionet_datapoint_timestamp_to_string(datapoint),
        bionet_hab_get_name(hab),
        bionet_node_get_id(node),
        bionet_resource_get_id(resource),
        value_str
    );

    free(value_str);
}


void cbbc_lost_node(bionet_node_t *node, void* usr_data) {
    g_message("%s - %s", 
        current_timestamp_string(),
        bionet_node_get_id(node));
}


void cbbc_new_node(bionet_node_t *node, void* usr_data) {
    int i;

    g_message("%s + %s", 
        current_timestamp_string(),
        bionet_node_get_id(node));

    if (bionet_node_get_num_resources(node)) {
        for (i = 0; i < bionet_node_get_num_resources(node); i++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
	    if (NULL == resource) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource at index %d from node", i);
		continue;
	    }
            bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);

            if (datapoint == NULL) {
                g_message(
                    "    %s %s %s ?",
                    bionet_resource_get_id(resource),
                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
                );
            } else {
                char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

                g_message(
                    "    %s %s %s '%s'",
                    bionet_resource_get_id(resource),
                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                    value_str
                );

		free(value_str);
            }

        }
    }

    if (bionet_node_get_num_streams(node)) {
        for (i = 0; i < bionet_node_get_num_streams(node); i++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            if (NULL == stream) {
                g_log("", G_LOG_LEVEL_WARNING, "Failed to get stream at index %d from node", i);
            }
        }
    }
}


void cbbc_lost_hab(bionet_hab_t *hab, void* usr_data) {
}


void cbbc_new_hab(bionet_hab_t *hab, void* usr_data) {
}

void cbbc_lost_bdm(bionet_bdm_t *bdm, void* usr_data) {
}


void cbbc_new_bdm(bionet_bdm_t *bdm, void* usr_data) {
}

void bdm_client_output_register_callbacks(void) {
    // register callbacks
    bdm_register_callback_new_bdm(cbbc_new_bdm, NULL);
    bdm_register_callback_lost_bdm(cbbc_lost_bdm, NULL);

    bdm_register_callback_new_hab(cbbc_new_hab, NULL);
    bdm_register_callback_lost_hab(cbbc_lost_hab, NULL);

    bdm_register_callback_new_node(cbbc_new_node, NULL);
    bdm_register_callback_lost_node(cbbc_lost_node, NULL);
    bdm_register_callback_datapoint(cbbc_datapoint, NULL);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
