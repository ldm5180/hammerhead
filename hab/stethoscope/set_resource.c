
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include "hardware-abstractor.h"


extern uint32_t gain;

extern bionet_node_t *node;


void cb_set_resource(bionet_resource_t * resource, bionet_value_t * value)
{
    bionet_datapoint_t * datapoint;
    struct timeval tv;

    if ((NULL == resource) || (NULL == value)) {
	g_warning("cb_set_resource(): NULL passed in");
    }

    char * value_str = bionet_value_to_str(value);
    g_log("", G_LOG_LEVEL_DEBUG, "setting %s to %s", 
	  bionet_resource_get_local_name(resource), 
	  value_str);
    free(value_str);


    datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
    if (datapoint) {
	bionet_datapoint_set_value(datapoint, value);
    } else {
	datapoint = bionet_datapoint_new(resource, value, NULL);
	bionet_resource_add_datapoint(resource, datapoint);
    } 

    bionet_resource_get_uint32(resource, &gain, &tv);

    hab_report_datapoints(node);
}
