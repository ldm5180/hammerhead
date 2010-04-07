
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <hardware-abstractor.h>

#include "syshealth.h"

static char *block_names[100];
static int number_of_blocks = 0;

static int parse_for_temperature(char* system_command);


static int smart_get(char* drive) {
    // Precondition: a character string containing the block name
    // Postcondition: returns the temperature of the drive

    FILE* fd;
    int temperature = -275;
    int r;
    char command[1000];
    int disk_number;

    // First pass: "normal" device
    snprintf(command, sizeof(command), "which smartctl > /dev/null && smartctl -A /dev/%s | grep -i temperature | perl -ne 'split && print \"$_[9]\n\"'", drive);
    temperature = parse_for_temperature(command);
    if (temperature > -275)
        return temperature;
    
    //  Second pass: ata device
    snprintf(command, sizeof(command), "which smartctl > /dev/null && smartctl -A -d ata /dev/%s | grep -i temperature | perl -ne 'split && print \"$_[9]\n\"'", drive);
    temperature = parse_for_temperature(command);
    if (temperature > -275)
        return temperature;
    
    
    // Third Pass: 3ware device
    snprintf(command, sizeof(command), "which smartctl > /dev/null && smartctl -a /dev/%s | grep -i Logical | perl -ne 'split && print \"$_[4]\"'", drive);
    fd = popen(command, "r");
    if (fd == NULL) {
	g_log("", G_LOG_LEVEL_WARNING, "Problem using popen: %s", strerror(errno));
        return -275;  // FIXME: bogus
    }

    r = fscanf(fd, "%d", &disk_number);
    pclose(fd);

    if (r == 1) {
    	// (device is 3ware), so use this command:
	
	snprintf(command, sizeof(command), "which smartctl > /dev/null && smartctl -A -d 3ware,%d /dev/twe0 | grep -i temperature | perl -ne 'split && print \"$_[9]\n\"'", disk_number);
	temperature = parse_for_temperature(command);
	if (temperature > -275)
	    return temperature;
    }
    

    // 
    // if we're here then something is broken
    //
    
    g_log("", G_LOG_LEVEL_WARNING, "Unable to retrieve temperature for %s", drive);

    return -275;
}

int parse_for_temperature(char* system_command) {
    // Precondition: system_command is a valid system call 
    // Postcondition: if the system call returned a number, then this
    //                number is returned.  If the system call was 
    //                unsuccessful, then -275 is returned.
    
    FILE* fd;
    int r, temperature;
    
    fd = popen(system_command, "r");
    if (fd == NULL) {
	g_log("", G_LOG_LEVEL_WARNING, "Problem using popen: %s", strerror(errno));
        return -275; // FIXME: -275 < absolute zero; this test failed
    }
    
    r = fscanf(fd, "%d", &temperature);
    pclose(fd);
    
    if (r == 1)
	return temperature;
    return -275; // -275 < absolute zero; this test failed
}



// FIXME: if smartctl is not installed, fail without ugly errors
int smart_init(bionet_node_t *node) {
    //  initializes everything.
    //  sets the names and numbers of each drive

    int r;
    DIR* directory;
    FILE* fd;

    number_of_blocks = 0;

    if (geteuid() != 0) {
        g_log("", G_LOG_LEVEL_WARNING, "not running as root, disabling Syshealth SMART harddisk temperature module");
        return -1;
    }

    directory = opendir("/sys/block/");
    if (directory == NULL) {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to open directory /sys/blocks/, disabling Syshealth SMART harddisk temperature module");
	return -1;
    }

    while (1) {
	char name[256], removable_drive_location[100];
	struct dirent* directory_info;
	int temperature, is_removable;
        bionet_resource_t *resource;
	
	directory_info = readdir(directory);
	if (directory_info == NULL)
	    break;

	if (directory_info->d_name[0] == '\0') {
	    g_log("", G_LOG_LEVEL_ERROR, "directory name is NULL");
	    break;
	}
	
	if (!(('h' == directory_info->d_name[0]) || ('s' == directory_info->d_name[0])))
	    continue;
	
	// If the drive is removable, skip it
	r = snprintf(removable_drive_location, sizeof(removable_drive_location), "/sys/block/%s/removable", directory_info->d_name);
	if (r == 0) {
	    g_log("", G_LOG_LEVEL_ERROR, "sprintf unable to create the filename");
	} else {
	    fd = fopen(removable_drive_location, "r");
	    if (fd == NULL) {
		g_log("", G_LOG_LEVEL_ERROR, "unable to open: %s", removable_drive_location);
	    } else {
		r = fscanf(fd, "%1d", &is_removable);
                fclose(fd);
		if (r != 1) {
		    g_log("", G_LOG_LEVEL_ERROR, "unable to scan %s (fscanf status: %s)", removable_drive_location, strerror(errno));
		} else {
		    if (is_removable == 1) continue;
                }
	    }
	}


	r = snprintf(name, sizeof(name), "%s-temperature", directory_info->d_name);
	if (r == 0) {
	    g_log("", G_LOG_LEVEL_ERROR, "unable to create resource name");
	}
	
	temperature = smart_get(directory_info->d_name);
        if (temperature == -275) {
            // couldn't read temp
            continue;
        }
	
	block_names[number_of_blocks] = strdup(directory_info->d_name);
	if (block_names[number_of_blocks] == NULL) {
	    g_log("", G_LOG_LEVEL_ERROR, "strdup failed, insufficient memory avalaible (ignoring %s", name);
	    continue;
	}

        // each hd gets its own resource
        resource = bionet_resource_new(node,
                BIONET_RESOURCE_DATA_TYPE_INT16,
                BIONET_RESOURCE_FLAVOR_SENSOR, 
                name);
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "smart_init(): error creating resource '%s'", name);
	    closedir(directory);
            return -1;
        }

        r = bionet_resource_set_int16(resource, temperature, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "smart_init(): error setting resource \'%s\'", name);
            continue;
	}

        r = bionet_node_add_resource(node, resource);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "smart_init(): node unable to add resource \'%s\'", name);
	} else {
            number_of_blocks++;
        }

    }

    r = closedir(directory);
    if (r != 0) {
	g_log("", G_LOG_LEVEL_WARNING, "could not close directory for some odd reason: %s", strerror(errno));
    }

    if (number_of_blocks > 0) { 
        return 0;
    } else {
        // none of the block devices gave us SMART responses...
        return -1;
    }
}


void smart_update(bionet_node_t *node) {
    //Precondition: None
    //Postcondition: For each block, sets the parameter hd temperature

    int r, i;

    // for (every hda/sda)
    for (i = 0; i < number_of_blocks; i++) {
	char resource_name[1000];
	int temperature_value;
        bionet_resource_t *resource;

	//printf("%s\n", block_names[i]);
	temperature_value = smart_get(block_names[i]);

	r = snprintf(resource_name, sizeof(resource_name), "%s-temperature", block_names[i]);
	if (r == 0) {
	    g_log("", G_LOG_LEVEL_ERROR, "smart_update(): unable to create resource name");
	}

        resource = bionet_node_get_resource_by_id(node, "Seconds-of-Uptime");
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "smart_update(): unable to find resource \'%s\'", resource_name);
            return;
        }

        r = bionet_resource_set_int16(resource, temperature_value, NULL);
	if (r < 0)
	    g_log("", G_LOG_LEVEL_WARNING, "smart_update(): Could not set resource value for %s", resource_name);
	else 
	    g_log("", G_LOG_LEVEL_DEBUG, "smart_update(): set %s to: %d\n", resource_name, temperature_value);
    }
}

