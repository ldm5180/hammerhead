
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

#include <glib.h>
#include <hardware-abstractor.h>

#include "syshealth.h"


// FIXME: disk_free returns a different value than the command df -m'


// 
// Syshealth autodetects all mounted block devices and tracks their free
// space.  During program startup we find the list of mounted partitions,
// each gets a partition_t in the partition array.
//

typedef struct {
    char *mount_point;
    char *mb_free_resource_id;
    char *inodes_free_resource_id;
} partition_t;

static partition_t *partition = NULL;
static int num_partitions = 0;

#ifdef LINUX
static void get_partition_list(void) {
    FILE *f = NULL;
    char tmp[100];

    f = fopen("/etc/mtab", "r");
    if (f == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "error opening /etc/mtab: %s", strerror(errno));
        return;
    }

    while (! feof(f)) {
        char *start;
        partition_t *part;

        if (fgets(tmp, sizeof(tmp), f) == NULL) break;

	char * p;
        if (strncmp(tmp, "/dev/", 5) != 0) continue;

        start = memchr(tmp, ' ', strlen(tmp));
        if (start == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "error parsing mtab line: %s", tmp);
            continue;
        }
        start ++;

        p = memchr(start, ' ', strlen(start));
        if (p == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "error parsing mtab line: %s", tmp);
            continue;
        }
        *p = '\0';

        num_partitions ++;

        partition = (partition_t *)realloc(partition, num_partitions * sizeof(partition_t));
        if (partition == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "out of memory allocating partitions array!");
            fclose(f);
            return;
        }
        part = &partition[num_partitions - 1];
	part->mount_point = strdup(start);

        // MB free resource id
        {
            char rid[256];
            int r;

            if (strcmp(start, "/") == 0) { 
                r = snprintf(rid, sizeof(rid), "MB-free-on-root");
		if (sizeof(rid) <= r) {
		    g_warning("get_partition_list(): Not enough space in dest buf of snprintf for MB-free-on-root.");
		    continue;
		}
            } else {
                char *p;

                start ++;

                for (p = start; *p != '\0'; p ++) {
                    if (*p == '/') {
                        *p = '-';
                    }
                }

                r = snprintf(rid, sizeof(rid), "MB-free-on-%s", start);
                if (r >= sizeof(rid)) {
                    g_log("", G_LOG_LEVEL_WARNING, "partition %s resource id too long!", part->mount_point);
                    continue;
                }
            }

            part->mb_free_resource_id = strdup(rid);
        }

        // Inodes free resource id
        {
            char rid[256];
            int r;

            if (strcmp(start, "/") == 0) { 
                r = snprintf(rid, sizeof(rid), "inodes-free-on-root");
		if (sizeof(rid) <= r) {
		    g_warning("get_partition_list(): Not enough space in dest buf of snprintf for inodes-free-on-root.");
		    continue;
		}
            } else {
                char *p;
		
                for (p = start; *p != '\0'; p ++) {
                    if (*p == '/') {
                        *p = '-';
                    }
                }

                r = snprintf(rid, sizeof(rid), "inodes-free-on-%s", start);
                if (r >= sizeof(rid)) {
                    g_log("", G_LOG_LEVEL_WARNING, "partition %s resource id too long!", part->mount_point);
                    continue;
                }
            }

            part->inodes_free_resource_id = strdup(rid);
        }
    }

    fclose(f);
}
#endif

#ifdef MACOSX
static void get_partition_list(void) {
    FILE * fp;
    char tmp[256];
    char partname[1024];
    char tmppartname[1024];
    char rid[1024];

    fp = popen("ls -1 /Volumes", "r");
    
    while (! feof(fp)) {
	partition_t * part;

	memset(partname, 0, sizeof(partname));
	memset(tmppartname, 0, sizeof(tmppartname));
	memset(tmp, 0, sizeof(tmp));

	if (fgets(tmp, sizeof(tmp), fp) == NULL) break;

	int i = 0;
	while(tmp[i]) {
	    if (tmp[i] == '\n') {
		tmp[i] = '\0';
		break;
	    }
	    i++;
	}

	int r = snprintf(partname, sizeof(partname), "/Volumes/%s", tmp);
	if (r >= sizeof(partname)) break;;

	partition = (partition_t *)realloc(partition, (num_partitions+1) * sizeof(partition_t));
	part = &partition[num_partitions];

	i = 0;
	int j = 0;
	while(partname[i]) {
	    if (partname[i] == ' ') {
		tmppartname[j] = '\\';
		j++;
		tmppartname[j] = partname[i];
		j++;
		partname[i] = '_';
		i++;
	    } else {
		tmppartname[j] = partname[i];
		j++;
		i++;
	    }
	}
	part->mount_point = strdup(tmppartname);
	num_partitions++;

	r = snprintf(rid, sizeof(rid), "MB-free-on-%s", partname);
	if (r >= sizeof(rid)) {
	    g_log("", G_LOG_LEVEL_WARNING, "partition %s resource id too long!", part->mount_point);
	    part->mb_free_resource_id = "NULL";
	    continue;
	}
	part->mb_free_resource_id = strdup(rid);
    }

    pclose(fp);
}
#endif

#ifdef LINUX
static int disk_free_get(char location[], int *mb_free, int *inodes_free) {
    //Precondition: input a string of the harddrive sector; ie., "/";
    //Postcondition: Returns 0 if it worked, -1 if it didnt work.
    
    struct statvfs buff;

    if (statvfs(location, &buff) != 0)
    {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to open: %s \n", location);
	return -1;
    }
    
    *mb_free = ((uint64_t)buff.f_bsize * (uint64_t)buff.f_bfree) / (uint64_t)(1024 * 1024);
    *inodes_free = buff.f_ffree;

    return 0;
}
#endif

#ifdef MACOSX
static int disk_free_get(char location[], int *mb_free, int *inodes_free) {
    FILE * fp;
    char freemem[1024];
    char cmd[1024];
    int r;

    r = snprintf(cmd, sizeof(cmd), "df -m %s | grep -v \"Available\" | awk '{print $4;}'", location);
    if (r >= sizeof(cmd)) {
	g_warning("Unable to create command to read disk free for %s: %m", location);
	return -1;
    }

    fp = popen(cmd, "r");
    r = fread(freemem, 1, sizeof(freemem), fp);
    if (0 >= r) {
	g_warning("Unable to read disk free for %s: %m", location);
	pclose(fp);
	return -1;
    }

    *mb_free = strtol(freemem, NULL, 0);

    pclose(fp);
    return 0;
}
#endif





int disk_free_init(bionet_node_t *node) {
    int i;
    int added_some = 0;


    get_partition_list();


    for (i = 0; i < num_partitions; i ++) {
        int r;
        int mb_free;
        int inodes_free;
        partition_t *part = &partition[i];
        bionet_resource_t *resource;

        if (disk_free_get(part->mount_point, &mb_free, &inodes_free) != 0) {
            continue;
        }


        resource = bionet_resource_new(node,
                BIONET_RESOURCE_DATA_TYPE_INT32,
                BIONET_RESOURCE_FLAVOR_SENSOR,
                part->mb_free_resource_id);
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): error creating resource %s", part->mb_free_resource_id);
            continue;
        }

        r = bionet_resource_set_int32(resource, mb_free, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): error setting resource %s", part->mb_free_resource_id);
            continue;
        }

        r = bionet_node_add_resource(node, resource);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): node unable to add resource %s", part->mb_free_resource_id);
            continue;
        }


        added_some = 1;

#ifdef LINUX
        resource = bionet_resource_new(node,
                BIONET_RESOURCE_DATA_TYPE_INT32,
                BIONET_RESOURCE_FLAVOR_SENSOR,
                part->inodes_free_resource_id);
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): error creating resource %s", part->inodes_free_resource_id);
            continue;
        }

        r = bionet_resource_set_int32(resource, inodes_free, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): error setting resource %s", part->inodes_free_resource_id);
            continue;
        }

        r = bionet_node_add_resource(node, resource);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): node unable to add resource %s", part->inodes_free_resource_id);
            continue;
        }
#endif

    }

    if (added_some) return 0;
    else return -1;
}




// 
// Update the disk-free resource values for all the partitions.
//

void disk_free_update(bionet_node_t *node) {
    int i;

    for (i = 0; i < num_partitions; i ++) {
        int mb_free;
        int inodes_free;
        int r;
        partition_t *part = &partition[i];
        bionet_resource_t *resource;

        if (disk_free_get(part->mount_point, &mb_free, &inodes_free) != 0) {
            continue;
        }


        resource = bionet_node_get_resource_by_id(node, part->mb_free_resource_id);
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_update(): looking for non-existent resource %s", part->mb_free_resource_id);
            continue;
        }

        r = bionet_resource_set_int32(resource, mb_free, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_update(): error setting value for resource %s", part->mb_free_resource_id);
        }

#ifdef LINUX
        resource = bionet_node_get_resource_by_id(node, part->inodes_free_resource_id);
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_update(): looking for non-existent resource %s", part->inodes_free_resource_id);
            continue;
        }

        r = bionet_resource_set_int32(resource, inodes_free, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_update(): error setting value for resource %s", part->inodes_free_resource_id);
        }
#endif

    }
}


