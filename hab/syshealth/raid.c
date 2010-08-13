
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <glib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "bionet-util.h"

GSList * md_devs = NULL;

int raid_init(bionet_node_t *node) {
    FILE * fp;
    char mdname[BIONET_NAME_COMPONENT_MAX_LEN];
    bionet_resource_t * resource;
    int ret;

    if (geteuid() != 0) {
        g_log("", G_LOG_LEVEL_WARNING, "not running as root, disabling Syshealth SMART harddisk temperature module");
        return -1;
    }

    if (system("grep -e \"^md[0-9+]\" /proc/mdstat | cut -d\" \" -f1 > /tmp/syshealth-hab.mdlist")) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to grep /proc/mdstat: %m");
	return -1;
    }
    fp = fopen("/tmp/syshealth-hab.mdlist", "r");
    if (NULL == fp) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to open /tmp/syshealth-hab.mdlist: %m");
	return -1;
    }

    while (fgets(mdname, BIONET_NAME_COMPONENT_MAX_LEN, fp)) {
	char command[512];
	char resname[BIONET_NAME_COMPONENT_MAX_LEN];

	mdname[strlen(mdname)-1] = '\0';

	if (1 != bionet_is_valid_name_component(mdname)) {
	    g_warning("%s is not a valid Bionet name component.", mdname);
	    continue;
	}

	if (BIONET_NAME_COMPONENT_MAX_LEN <= snprintf(resname, BIONET_NAME_COMPONENT_MAX_LEN, "dev-%s", mdname)) {
	    g_log("", G_LOG_LEVEL_WARNING, "%s is too long", mdname);
	    continue;
	}

	md_devs = g_slist_append(md_devs, strdup(mdname));

	resource = bionet_resource_new(node,
				       BIONET_RESOURCE_DATA_TYPE_STRING,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       resname);
	if (NULL == resource) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to create resource: %s", resname);
	    continue;
	}

	snprintf(command, sizeof(command), "mdadm --detail --test /dev/%s > /dev/null", mdname);

	ret = system(command);
	if (WEXITSTATUS(ret)) {
	    g_log("", G_LOG_LEVEL_WARNING, "/dev/%s is degraded or failed", mdname);
	    bionet_resource_set_str(resource, "Degraded", NULL);
	} else {
	    bionet_resource_set_str(resource, "Normal", NULL);
	}
	
	if (bionet_node_add_resource(node, resource)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to add resource %s to node %s",
		  bionet_resource_get_id(resource), bionet_node_get_name(node));
	}
    } /* while(fgets...) */

    fclose(fp);

    return 0;
} /* raid_init() */


void raid_update(bionet_node_t *node) {
    int i;
    char * mdname;
    char resname[BIONET_NAME_COMPONENT_MAX_LEN];
    bionet_resource_t * resource;
    int ret;

    for (i = 0; i < g_slist_length(md_devs); i++) {
	char command[512];

	mdname = g_slist_nth_data(md_devs, i);
	snprintf(resname, BIONET_NAME_COMPONENT_MAX_LEN, "dev-%s", mdname);

	resource = bionet_node_get_resource_by_id(node, resname);
	if (NULL == resource) {
	    g_log("", G_LOG_LEVEL_WARNING, "Unable to find resource: %s", resname);
	    continue;
	}

	snprintf(command, sizeof(command), "mdadm --detail --test /dev/%s > /dev/null", mdname);
	ret = system(command);
	if (WEXITSTATUS(ret)) {
	    g_log("", G_LOG_LEVEL_WARNING, "/dev/%s is degraded or failed", mdname);
	    bionet_resource_set_str(resource, "Degraded", NULL);
	} else {
	    bionet_resource_set_str(resource, "Normal", NULL);
	}	
    }
} /* raid_update() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
