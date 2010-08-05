
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <glib.h>

#include "bdm-client.h"
#include "bionet.h"
   
void cb_bdm_datapoint_csv(bionet_datapoint_t *datapoint, bionet_event_t * event, void * usr_data) {
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
    char * time_str = bionet_event_get_timestamp_as_str(event);
    const char * bdm_id = bionet_event_get_bdm_id(event);
    if(bdm_id == NULL) bdm_id = "LIVE";

    g_message(
        "%s,+D,%s,%s %s %s @ %s,%s",
        time_str,
        bionet_resource_get_name(resource),
        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
        value_str,
        bionet_datapoint_timestamp_to_string(datapoint),
        bdm_id
    );

    free(value_str);
    free(time_str);
}


void cb_bdm_lost_node_csv(bionet_node_t *node, bionet_event_t * event, void* usr_data) {
    char * time_str = bionet_event_get_timestamp_as_str(event);
    const char * bdm_id = bionet_event_get_bdm_id(event);
    if(bdm_id == NULL) bdm_id = "LIVE";
    g_message(
        "%s,-N,%s,,%s",
        time_str,
        bionet_node_get_name(node),
        bdm_id
    );
    free(time_str);
}


void cb_bdm_new_node_csv(bionet_node_t *node, bionet_event_t * event, void* usr_data) {
    int i;
    char * time_str = bionet_event_get_timestamp_as_str(event);
    const char * bdm_id = bionet_event_get_bdm_id(event);
    if(bdm_id == NULL) bdm_id = "LIVE";
    g_message(
        "%s,+N,%s,,%s",
        time_str,
        bionet_node_get_name(node),
        bdm_id
    );

    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
        if (NULL == resource) {
            g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource at index %d from node", i);
            continue;
        }
        g_message(
            "%s,+N,%s,%s %s,%s",
            time_str,
            bionet_resource_get_name(resource),
            bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
            bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
            bdm_id
        );
    }

    if (bionet_node_get_num_streams(node)) {
        for (i = 0; i < bionet_node_get_num_streams(node); i++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            if (NULL == stream) {
                g_log("", G_LOG_LEVEL_WARNING, "Failed to get stream at index %d from node", i);
            }

            g_message(
                "%s,+N,%s,%s %s,%s",
                time_str,
                bionet_stream_get_name(stream),
                bionet_stream_get_type(stream),
                bionet_stream_direction_to_string(bionet_stream_get_direction(stream)),
                bdm_id
            );
        }
    }
    free(time_str);
}


void cb_bdm_lost_hab_csv(bionet_hab_t *hab, bionet_event_t * event, void* usr_data) {
    char * time_str = bionet_event_get_timestamp_as_str(event);
    const char * bdm_id = bionet_event_get_bdm_id(event);
    if(bdm_id == NULL) bdm_id = "LIVE";
    g_message(
        "%s,-H,%s,,%s",
        time_str,
        bionet_hab_get_name(hab),
        bdm_id
    );
    free(time_str);
}


void cb_bdm_new_hab_csv(bionet_hab_t *hab, bionet_event_t * event, void* usr_data) {
    char * time_str = bionet_event_get_timestamp_as_str(event);
    const char * bdm_id = bionet_event_get_bdm_id(event);
    if(bdm_id == NULL) bdm_id = "LIVE";
    g_message(
        "%s,+H,%s,%s",
        time_str,
        bionet_hab_get_name(hab),
        bdm_id
    );
    free(time_str);
}

void cb_bdm_lost_bdm_csv(bionet_bdm_t *bdm, void* usr_data) {
    g_message("lost bdm: %s", bionet_bdm_get_id(bdm));
}


void cb_bdm_new_bdm_csv(bionet_bdm_t *bdm, void* usr_data) {
    g_message("new bdm: %s", bionet_bdm_get_id(bdm));
}

// Bionet callbacks
void cb_new_hab_csv(bionet_hab_t * hab) {
    bionet_event_t * event = bionet_hab_get_event_by_index(hab, bionet_hab_get_num_events(hab)-1);
    cb_bdm_new_hab_csv(hab, event, NULL);
}
void cb_lost_hab_csv(bionet_hab_t * hab) {
    bionet_event_t * event = bionet_hab_get_event_by_index(hab, bionet_hab_get_num_events(hab)-1);
    cb_bdm_lost_hab_csv(hab, event, NULL);
}

void cb_new_node_csv(bionet_node_t * node) {
    bionet_event_t * event = bionet_node_get_event_by_index(node, bionet_node_get_num_events(node)-1);
    cb_bdm_new_node_csv(node, event, NULL);
}
void cb_lost_node_csv(bionet_node_t * node) {
    bionet_event_t * event = bionet_node_get_event_by_index(node, bionet_node_get_num_events(node)-1);
    cb_bdm_lost_node_csv(node, event, NULL);
}

void cb_datapoint_csv(bionet_datapoint_t * datapoint) {
    bionet_event_t * event = bionet_datapoint_get_event_by_index(datapoint, bionet_datapoint_get_num_events(datapoint)-1);
    cb_bdm_datapoint_csv(datapoint, event, NULL);
}



void csv_output_register_callbacks(void) {
    // register callbacks
    bionet_register_callback_new_hab(cb_new_hab_csv);
    bionet_register_callback_lost_hab(cb_lost_hab_csv);

    bionet_register_callback_new_node(cb_new_node_csv);
    bionet_register_callback_lost_node(cb_lost_node_csv);

    bionet_register_callback_datapoint(cb_datapoint_csv);
}

void csv_output_register_bdm_callbacks(void) {
    // register callbacks
    bdm_register_callback_new_bdm(cb_bdm_new_bdm_csv, NULL);
    bdm_register_callback_lost_bdm(cb_bdm_lost_bdm_csv, NULL);

    bdm_register_callback_new_hab(cb_bdm_new_hab_csv, NULL);
    bdm_register_callback_lost_hab(cb_bdm_lost_hab_csv, NULL);

    bdm_register_callback_new_node(cb_bdm_new_node_csv, NULL);
    bdm_register_callback_lost_node(cb_bdm_lost_node_csv, NULL);

    bdm_register_callback_datapoint(cb_bdm_datapoint_csv, NULL);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
