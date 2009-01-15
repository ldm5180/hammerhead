
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include <hardware-abstractor.h>

#include "pal-650-hab.h"


#if 0
static void hexdump(char *buffer, int size) {
    char hex[60];
    char ascii[20];
    int byte, hex_dst, ascii_dst;

    hex_dst = 0;
    ascii_dst = 0;

    for (byte = 0; byte < size; byte ++) {
        sprintf(&hex[hex_dst], "%02X ", buffer[byte]);
        hex_dst += 3;

        if (isprint(buffer[byte])) {
            sprintf(&ascii[ascii_dst], "%c", buffer[byte]);
        } else {
            sprintf(&ascii[ascii_dst], ".");
        }

        ascii_dst ++;

        if (byte % 16 == 15) {
            printf("    %s |%s|\n", hex, ascii);
            hex_dst = 0;
            ascii_dst = 0;
        } else if (byte % 8 == 7) {
            sprintf(&hex[hex_dst], " ");
            hex_dst ++;
            sprintf(&ascii[ascii_dst], " ");
            ascii_dst ++;
        }
    }

    if (byte % 16 != 0) {
        printf("    %-49s |%-17s|\n", hex, ascii);
    }
}
#endif

static void parse_line(char *line) {
    char tag_id[16];
    float x_coord;
    float y_coord;
    float z_coord;

    int i = 0;
    char delims[] = ",";
    char *result = NULL;

    bionet_node_t *node = 0;


    printf(" Parsing line: %s\n", line);

    memset(tag_id, '\0', sizeof(tag_id));

    result = strtok(line, delims);

    while (result != NULL) {
        printf("(%d) result = \"%s\"\n", i, result);

        if (strcmp(result, "P") == 0) {
            printf("Got a Presence indicator.\n");
        }

        switch(i) {
            case 1: 
                memcpy(tag_id, result, strlen(result));
                //printf("tag: %s\n", tag_id);
                break;

            case 2:
                x_coord = strtod(result, NULL);
                //printf("x_coord: %s\n", x_coord);
                break;

            case 3:
                y_coord = strtod(result, NULL);
                //printf("y_coord: %s, len: %d\n", y_coord, strlen(result));
                break;

            case 4:
                z_coord = strtod(result, NULL);
                //printf("z_coord: %s, len: %d\n", z_coord, strlen(result));
                break;
        }

        result = strtok(NULL, delims);
        i++;
    }

    node = g_hash_table_lookup(nodes, tag_id);
    if (node != NULL) {
        node_data_t *node_data = bionet_node_get_user_data(node);
        // aw, old node, we already knew about this one
        // refresh its last-seen time so we dont remove it just yet
        node_data->time = time(NULL);
    }

    printf("New Node '%s'\n", tag_id);

    node = bionet_node_new(hab, tag_id);

    if (node == NULL) {
        g_warning("Error creating a new node '%s'", tag_id);
        return;
    }


    {
        int i;
        char *resource_id[] = { "X", "Y", "Z" };
        float value[] = { x_coord, y_coord, z_coord };

        for (i = 0; i < 3; i ++) {
            bionet_resource_t *resource;

            resource = bionet_resource_new(
                node,
                BIONET_RESOURCE_DATA_TYPE_FLOAT,
                BIONET_RESOURCE_FLAVOR_SENSOR,
                resource_id[i]
            );
            if (resource == NULL) {
                g_warning("error making Resource %s:%s", bionet_node_get_id(node), resource_id[i]);
                bionet_node_free(node);
                return;
            }

            bionet_resource_set_float(resource, value[i], NULL);

            bionet_node_add_resource(node, resource);
        }
    }


    {
        node_data_t *node_data = (node_data_t*)calloc(1, sizeof(node_data_t));
        if (node_data == NULL) {
            g_warning("out of memory");
            bionet_node_free(node);
        }

        node_data->time = time(NULL);

        bionet_node_set_user_data(node, node_data);
    }


    hab_report_new_node(node);

    g_hash_table_insert(nodes, strdup(tag_id), node);
    
    printf("All looks good with '%s'\n", tag_id);
}

int pal_read(int pal_fd) {
    static char buffer[4096];
    static int index = 0;

    int r;


    r = read(pal_fd, &buffer[index], (sizeof(buffer) - index));

    if (r < 0) {
        g_warning("error reading from PAL: %s", strerror(errno));
        index = 0;

        return -1;
    }

    if (r == 0) {
        g_warning("PAL disconnected");
        index = 0;

        return -1;
    }

    //printf("read %d bytes:\n", r);
    //hexdump(&buffer[index], r);

    index += r;

	/*
	 * Parse all data within the buffer.
	 */
    while (1) {
        char *p;

        p = memchr(buffer, '\n', index);

        if (p == NULL) {
            // no '\n' in buffer, keep reading

            if (index > sizeof(buffer)) {
                // overflow!  truncate!
                index = 0;
            }

            return 0;
        }

		/*
		 * If we get here, then the buffer contains at least one complete line.
		 */
    	//printf("Got at least one complete line.\n");

        *p = '\0';

        parse_line(buffer);

        int remainder = index - ((p+1) - buffer);
        memmove(buffer, (p+1), remainder);
        index = remainder;

        //printf("buffer has %d bytes:\n", index);
        //hexdump(buffer, index);
    }

    return 0;
}

