
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
    char *resource_id;
} partition_t;

static partition_t *partition = NULL;
static int num_partitions = 0;


static void get_partition_list(void) {
    FILE *f;
    char tmp[100];

    f = fopen("/etc/mtab", "r");
    if (f == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "error opening /etc/mtab: %s", strerror(errno));
        return;
    }

    while (! feof(f)) {
        char *start, *p;
        partition_t *part;

        if (fgets(tmp, sizeof(tmp), f) == NULL) break;

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

        {
            char rid[256];
            int r;

            if (strcmp(start, "/") == 0) { 
                r = snprintf(rid, sizeof(rid), "MB-free-on-root");

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

            part->resource_id = strdup(rid);
        }
    }

    fclose(f);
}


static int disk_free_get(char location[]) {
    //Precondition: input a string of the harddrive sector; ie., "/";
    //Postcondition: Returns the MB free as an int
    //The function statvfs from the library <sys/statvfs.h>
    //returns a struct containing (only noting the two imports/used
    //variables:
    //   f_bsize: the file system blocksize
    //   f_bfree: the # of free blocks
    
    struct statvfs buff;
    int disk_fr = -1;
    
    if (statvfs(location, &buff) != 0)
    {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to open: %s \n", location);
	return -1;
    }
    
    return disk_fr = ((uint64_t)buff.f_bsize * (uint64_t)buff.f_bfree) / (uint64_t)(1024 * 1024);
}






int disk_free_init(bionet_node_t *node) {
    int i;
    int added_some = 0;


    get_partition_list();


    for (i = 0; i < num_partitions; i ++) {
        int r;
        int diskfree;
        bionet_resource_t *resource;
        partition_t *part = &partition[i];

        diskfree = disk_free_get(part->mount_point);

        resource = bionet_resource_new(node,
                BIONET_RESOURCE_DATA_TYPE_INT32, 
                BIONET_RESOURCE_FLAVOR_SENSOR, 
                part->resource_id);
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): error creating resource %s", part->resource_id);
            continue;
        }

        r = bionet_resource_set_int32(resource, diskfree, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): error setting resource %s", part->resource_id);
        } else {
            added_some = 1;
        }

        r = bionet_node_add_resource(node, resource);
        if (r < 0)
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): node unable to add resource \'%s\'", part->resource_id);
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
        int diskfree;
        int r;
        partition_t *part = &partition[i];
        bionet_resource_t *resource;

        diskfree = disk_free_get(part->mount_point);

        resource = bionet_node_get_resource_by_id(node, part->resource_id);
        if (resource == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_update(): looking for non-existent resource %s", part->resource_id);
            continue;
        }

        r = bionet_resource_set_int32(resource, diskfree, NULL);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "disk_free_update(): error setting value for resource %s", part->resource_id);
        }
    }
}


