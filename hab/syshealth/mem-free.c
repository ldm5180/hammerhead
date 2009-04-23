
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <hardware-abstractor.h>

#include "syshealth.h"




static int mem_free_get(void) {
    // Precondition: none
    // Returns the KB memory free as an int
    // Reads from the file meminfo; in '/proc/meminfo'
    // which has the format (all values are ints):
    //             total:        used:             free:     shared:     buffers:  cached:
    // Mem:    [amt of Mem] [used amt of mem]  [mem free]  [mem shared]  [buffers] [cache]
    // swap:   [total swap] [used amt of swap] [swap free] [swap shared] [buffers] [cache]  
    // MemTotal:   [mem total]     kB
    // MemFree:    [mem free]      kB
    // MemShared:  [mem shared]    kB
    // Buffers:    [buffers]       kB
    // Cached:     [cache]         kB
    // SwapCached: [swapped cache] kB
    // Active:     [active]        kB
    // Inactive:   [inactive]      kB
    // Hightotal:  [hightotal]     kB
    // Highfree:   [highfree]      kB
    // LowTotal:   [lowtotal]      kB
    // LowFree:    [lowfree]       kB
    // SwapTotal:  [SwapTotal]     kB
    // SwapFree:   [SwapFree]      kB
    // This function reads from 'MemFree', 'Buffers',
    // and 'Cached' and addes them to determine the 
    // total number of KB of Memory free
    
    FILE *fb;
    int total_mem_free = -1;
    int buffers_free = -1;
    int cache = -1;
    char temp_loc[1000];           //FIXME: What if temp_loc is too small???
    
    
    fb = fopen("/proc/meminfo", "r");
    if (!fb) 
    {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to open file: /proc/meminfo\n");
	return -1;
    }
    while (fscanf(fb, "%1000s", temp_loc) != EOF)
    {
	if (!strncmp(temp_loc, "MemFree:", 8)) {
            if (fscanf(fb, "%9d", &total_mem_free) != 1) {
                g_log("", G_LOG_LEVEL_WARNING, "unable to read MemFree value from /proc/meminfo\n");
		fclose(fb);
                return -1;
            }
        } else if (!strncmp(temp_loc, "Buffers:", 8)) {
            if (fscanf(fb, "%9d", &buffers_free) != 1) {
                g_log("", G_LOG_LEVEL_WARNING, "unable to read Buffers value from /proc/meminfo\n");
		fclose(fb);
                return -1;
            }
	} else if (!strncmp(temp_loc, "Cached:", 7)) {
            if (fscanf(fb, "%9d", &cache) != 1) {
                g_log("", G_LOG_LEVEL_WARNING, "unable to read Cached value from /proc/meminfo\n");
		fclose(fb);
                return -1;
            }
        }
    }
    fclose(fb);
    
    if ((total_mem_free == -1) || (buffers_free == -1) || (cache == -1))
    {
	g_log("", G_LOG_LEVEL_WARNING, "Unable to add the correct amount of memory free in file:");
	g_log("", G_LOG_LEVEL_WARNING, "Memfree = %d; Buffers = %d; Cached = %d;", total_mem_free, buffers_free, cache);
	return -1;
    }
    
    return (total_mem_free + buffers_free + cache);
}




int mem_free_init(bionet_node_t *node) {
    int r;
    int mem;
    bionet_resource_t *resource;

    mem = mem_free_get();
    resource = bionet_resource_new(node,
            BIONET_RESOURCE_DATA_TYPE_UINT32, 
            BIONET_RESOURCE_FLAVOR_SENSOR, 
            "KB-memory-free");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "mem_free_init(): error creating resource KB-memory-free");
        return -1;
    }

    r = bionet_resource_set_uint32(resource, mem, NULL);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "mem_free_init(): error setting resource KB-memory-free");
        return r;
    }
        
    r = bionet_node_add_resource(node, resource);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "disk_free_init(): node unable to add resource");
        return r;
    }

    return 0;
}


void mem_free_update(bionet_node_t *node) {
    //Precondition: None
    //Postcondition: Sets the parameter 'KB memory free'
    
    int r;
    int KBfree;
    bionet_resource_t *resource;
    
    
    KBfree = mem_free_get();
    
    resource = bionet_node_get_resource_by_id(node, "KB-memory-free");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "mem_free_update(): unable to find resource KB-memory-free");
        return;
    }

    r = bionet_resource_set_uint32(resource, KBfree, NULL);
    
    if (r < 0) 
        g_log("", G_LOG_LEVEL_WARNING, "mem_free_update(): Could not set resource value for KB-memory-free"); 
    else 
        g_log("", G_LOG_LEVEL_DEBUG, "set KB memory free to: %d\n", KBfree);
}

