
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <hardware-abstractor.h>

#include "syshealth.h"




static float uptime_get(void) {
    // Precondition: None
    // Postcondition: Returns the computer uptime as a float
    //
    // This function reads from the file '/proc/uptime'; which has
    // the format:
    // [seconds uptime (float)] [amount of time the system has been idle (float)]
    //
    // This function reads from the first float to get the
    // second uptime.

    FILE *fd;
    float time;
    int r;

    
    fd=fopen("/proc/uptime", "r");
    if (!fd)
    {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to open file: /proc/uptime\n");
	return -1;
    }
    
    r = fscanf(fd, "%f", &time);
    fclose(fd);

    if (r != 1) {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to read uptime from /proc/uptime");
	return -1;
    }

    return time;
}




int uptime_init(bionet_node_t *node) {
    int r;
    float up;
    bionet_resource_t* resource;

    up = uptime_get();
    
    resource = bionet_resource_new(node,
            BIONET_RESOURCE_DATA_TYPE_FLOAT,
            BIONET_RESOURCE_FLAVOR_SENSOR, 
            "Seconds-of-uptime");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "uptime_init(): error creating resource \"Seconds-of-uptime\"");
        return -1;
    }

    r = bionet_resource_set_float(resource, up, NULL);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "uptime_init(): error setting resource \"Seconds-of-uptime\"");
        return r;
    }
    
    r = bionet_node_add_resource(node, resource);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "uptime_init(): error setting resource \"Seconds-of-uptime\"");
        return r;
    }

    return 0;
}


void uptime_update(bionet_node_t *node) {
    //Precondition: None
    //Postcondition: Sets the parameter 'Seconds of uptime'

    int r;
    float uptime_value;
    bionet_resource_t *resource;

    uptime_value = uptime_get();

    resource = bionet_node_get_resource_by_id(node, "Seconds-of-uptime");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "uptime_init(): unable to find resource Seconds-of-uptime");
        return;
    }

    r = bionet_resource_set_float(resource, uptime_value, NULL);
    if (r < 0)
        g_log("", G_LOG_LEVEL_WARNING, "uptime_init(): Could not set resource value for Seconds-of-uptime");
    else 
        g_log("", G_LOG_LEVEL_DEBUG, "uptime_init(): set Seconds of uptime to: %f\n", uptime_value);
}

