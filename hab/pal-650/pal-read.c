
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




// 
// The "packet" format from the PAL-650 is:
//
//     <Data Header>, <tag #>, <X>, <Y>, <Z>, <battery>, <timestamp>, <unit> [,<DQI>] <LF>
//
// Note DQI is optional.  This HAB ignores it if it *is* present.
//

typedef struct {
    char header;
    char tag_id[16];
    float x, y, z;
    int battery;
    time_t timestamp;
    char unit[16];
    char remainder[100];
} message_t;


static const char *resource_id[] = { "X", "Y", "Z" };




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




static bionet_node_t *get_node(const char *node_id) {
    bionet_node_t *node;
    node_data_t *node_data;

    node = bionet_hab_get_node_by_id(hab, node_id);
    if (node == NULL) {
        int i;

        printf("New Node '%s'\n", node_id);

        node = bionet_node_new(hab, node_id);
        if (node == NULL) {
            g_warning("Error creating a new node '%s'", node_id);
            return NULL;
        }

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
                return NULL;
            }

            bionet_node_add_resource(node, resource);
        }

        node_data = (node_data_t*)calloc(1, sizeof(node_data_t));
        if (node_data == NULL) {
            g_warning("out of memory");
            bionet_node_free(node);
            return NULL;
        }
        bionet_node_set_user_data(node, node_data);

        bionet_hab_add_node(hab, node);

        hab_report_new_node(node);
    }

    node_data = bionet_node_get_user_data(node);
    node_data->time = time(NULL);

    return node;
}




static void handle_diagnostic_message(const message_t *m) {
    g_message("diagnostic message from PAL-650: %s", m->remainder);
}



static void handle_presence_message(const message_t *m) {
    bionet_node_t *node;

    node = get_node(m->tag_id);
    if (node == NULL) return;
}





static void handle_position_message(const message_t *m) {
    bionet_node_t *node;
    float value[3] = { m->x, m->y, m->z };
    int i;

    node = get_node(m->tag_id);
    if (node == NULL) return;

    for (i = 0; i < 3; i ++) {
        bionet_resource_t *resource;

        resource = bionet_node_get_resource_by_id(node, resource_id[i]);
        if (resource == NULL) {
            g_warning("error getting Resource %s:%s", bionet_node_get_id(node), resource_id[i]);
            bionet_node_free(node);
            return;
        }

        bionet_resource_set_float(resource, value[i], NULL);
    }

    hab_report_datapoints(node);
}




static void parse_line(char *line) {
    message_t m;

    int i;
    char delims[] = ",";
    char *result = NULL;


    if (show_messages) {
        printf("Parsing line: %s\n", line);
    }

    m.header = line[0];
    line += 2;

    i = 1;
    result = strtok(line, delims);
    while (result != NULL) {
        switch(i) {
            case 1: 
                strncpy(m.tag_id, result, sizeof(m.tag_id));
                m.tag_id[sizeof(m.tag_id)-1] = '\0';
                break;

            case 2:
                m.x = strtod(result, NULL);
                break;

            case 3:
                m.y = strtod(result, NULL);
                break;

            case 4:
                m.z = strtod(result, NULL);
                break;

            case 5:
                m.battery = atoi(result);
                break;

            case 6:
                m.timestamp = atoi(result);
                break;

            case 7: 
                strncpy(m.unit, result, sizeof(m.unit));
                m.unit[sizeof(m.unit)-1] = '\0';
                break;

            case 8: 
                strncpy(m.remainder, result, sizeof(m.remainder));
                m.remainder[sizeof(m.remainder)-1] = '\0';
                break;

            default:
                g_warning("dont know what to do with field %d: '%s'", i, result);
                break;
        }

        result = strtok(NULL, delims);
        i++;
    }

    if (show_messages) {
        printf("    header=%c\n", m.header);
        printf("    tag_id=%s\n", m.tag_id);
        printf("    x=%.3g\n", m.x);
        printf("    y=%.3g\n", m.y);
        printf("    z=%.3g\n", m.z);
        printf("    battery=%d\n", m.battery);
        printf("    timestamp=%d\n", (int)m.timestamp);
        printf("    unit=%s\n", m.unit);
        printf("    remainder=%s\n", m.remainder);
    }

    switch (m.header) {
        case 'D': 
            return handle_diagnostic_message(&m);
            break;

        case 'R':
        case 'T':
        case 'O':
            return handle_position_message(&m);
            break;

        case 'P':
            return handle_presence_message(&m);
            break;

        default:
            g_warning("dont know what to do with '%c' message", m.header);
            break;
    }
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

    index += r;

    // Parse all data within the buffer.
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

        //
        // If we get here, then the buffer contains at least one complete
        // line, and p points to it.
        //

        *p = '\0';

        parse_line(buffer);

        int remainder = index - ((p+1) - buffer);
        memmove(buffer, (p+1), remainder);
        index = remainder;
    }

    return 0;
}

