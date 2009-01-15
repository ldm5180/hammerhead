
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include <hardware-abstractor.h>

#include "pal-650-hab.h"


#if 1
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

static void parse_line(const char *line) {
    int r1, r2, r3;
    char tag_id[16];
    char x_coord[16];
    char y_coord[16];
    char z_coord[16];

	bionet_node_t *node = 0;

    printf(" Parsing line: %s\n", line);

	int i = 0;
	char delims[] = ",";
	char *result = NULL;

	memset(tag_id, '\0', sizeof(tag_id));
	memset(x_coord, '\0', sizeof(x_coord));
	memset(y_coord, '\0', sizeof(y_coord));
	memset(z_coord, '\0', sizeof(z_coord));

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
				memcpy(x_coord, result, strlen(result));
				//printf("x_coord: %s\n", x_coord);

				break;

			case 3:
				memcpy(y_coord, result, strlen(result)); 
				//printf("y_coord: %s, len: %d\n", y_coord, strlen(result));

				break;

			case 4:
				memcpy(z_coord, result, strlen(result));
				//printf("z_coord: %s, len: %d\n", z_coord, strlen(result));

				break;
		}

		result = strtok(NULL, delims);
		i++;
	}

    node = g_hash_table_lookup(nodes, tag_id);
	printf("pnt 1\n");

    if (node != NULL) {
        // aw, old node, we already knew about this one
        // refresh its last-seen time so we dont remove it just yet
        time(node->user_data);
    }
	
   	printf("New Node '%s'\n", tag_id);

   	node = bionet_node_new(NULL, NULL, tag_id);

    if (node == NULL) {
        g_warning("Error creating a new node '%s'", tag_id);

        return;
    }

    //r1 = bionet_node_add_resource_with_valueptr_timevalptr(node, "float", 
	//	"Parameter", "Tag-X", &tag_x, NULL);
    r1 = bionet_node_add_resource_with_valuestr_timestr(node, "Float", 
		"Parameter", "X", x_coord, NULL);
    r2 = bionet_node_add_resource_with_valuestr_timestr(node, "Float", 
		"Parameter", "Y", y_coord, NULL);
	r3 = bionet_node_add_resource_with_valuestr_timestr(node, "Float", 
		"Parameter", "Z", z_coord, NULL);

    if ((r1 < 0) || (r2 < 0) || (r3 < 0)) {
        g_warning("Error adding data to node '%s', dropping node", tag_id);
        bionet_node_free(node);

        return;
    }

    node->user_data = malloc(sizeof(time_t));

    if (node->user_data == NULL) {
        g_warning("Error adding user_data to node '%s', dropping node", tag_id);
        bionet_node_free(node);

        return;
    }

    time(node->user_data);

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

        *p = (char)NULL;

        parse_line(buffer);

        int remainder = index - ((p+1) - buffer);
        memmove(buffer, (p+1), remainder);
        index = remainder;

        //printf("buffer has %d bytes:\n", index);
        //hexdump(buffer, index);
    }

    return 0;
}

