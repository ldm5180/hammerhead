
//
// Copyright (C) 2008-2009, the Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>
#include <hardware-abstractor.h>

#include "syshealth.h"




static float load_average(void) {
    // Precondition: None
    // Postcondition: Returns the 15 minute average load as a float
    //
    // This function reads from the file '/proc/loadavg' which 
    // has the format:
    // [number of active tasks averaged over the past minute (float)] [number
    // of active tasks averaged over the past five minutes (float)] [number of
    // active tasks averaged over the past 15 minutes (float)] [current number
    // of running tasks (int)] / [total number of processes on the system
    // (int)] [process ID of the most recently ran process (int)]
    //
    // This function returns the third float: the number of tasks 
    // averaged over the past 15 minutes.
    
    FILE *fd;
    float avgload;
    int r;

    
    fd=fopen("/proc/loadavg", "r");
    if (!fd) {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to open file: /proc/loadavg\n");
	return -1;
    }

    // this reads the first three floats in the file, discards the first
    // two, and assigns the third one to avgload
    r = fscanf(fd, "%*f %*f %f", &avgload);

    fclose(fd);

    if (r != 1) {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to read load average from /proc/loadavg");
	return -1;
    }
    
    return avgload;
}




int load_average_init(bionet_node_t *node) {
    int r;
    float load_avg;
    bionet_resource_t *resource;

    load_avg = load_average();

    resource = bionet_resource_new(node,
            BIONET_RESOURCE_DATA_TYPE_FLOAT,
            BIONET_RESOURCE_FLAVOR_SENSOR, 
            "15-min-load-average");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "load_average_init(): error creating resource 15-min-load-average");
        return -1;
    }

    r = bionet_resource_set_float(resource, load_avg, NULL);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "load_average_init(): error setting resource 15-min-load-average");
        return -1;
    }

    r = bionet_node_add_resource(node, resource);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "load_average_init(): node unable to add resource 15-min-load-average");
        return -1;
    }

    return 0;
}


void load_average_update(bionet_node_t *node) {
    //Precondition: None
    //Postcondition: Sets the parameter '15-minute load average'
	
    int r;
    float loadavg;
    bionet_resource_t *resource;
    
    loadavg = load_average();

    resource = bionet_node_get_resource_by_id(node, "15-min-load-average");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "unable to find resource 15-min-load-average");
        return;
    }

    r = bionet_resource_set_float(resource, loadavg, NULL);
    if (r < 0) 
        g_log("", G_LOG_LEVEL_WARNING, "Could not set resource value for 15-min-load-average");
    else 
        g_log("", G_LOG_LEVEL_DEBUG, "Set 15-minute load average to: %f\n", loadavg);
}

