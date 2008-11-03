
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#define _GNU_SOURCE  // for isnormal()

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <windows.h>
#endif

#include <glib.h>

#include "bionet-util.h"
#include "hardware-abstractor.h"

#include "random-hab.h"




void randomize_buffer(void *buffer, int size) {
    unsigned char *b = buffer;
    for ( ; size > 0; size --) {
        *b = rand() & 0xFF;
        b ++;
    }
}




void set_random_resource_value(bionet_resource_t* resource) {
    bionet_datapoint_t *datapoint;
    bionet_datapoint_value_t value;

    switch (resource->data_type) {
        case BIONET_RESOURCE_DATA_TYPE_INVALID: {
            printf("*** INVALID DATA TYPE\n");
            return;
        }

        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            value.binary_v = rand() % 2;
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            randomize_buffer(&value.uint8_v, sizeof(uint8_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            randomize_buffer(&value.int8_v, sizeof(int8_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            randomize_buffer(&value.uint16_v, sizeof(uint16_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            randomize_buffer(&value.int16_v, sizeof(int16_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            randomize_buffer(&value.uint32_v, sizeof(uint32_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            randomize_buffer(&value.int32_v, sizeof(int32_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            do {
                randomize_buffer(&value.float_v, sizeof(float));
            } while (!isnormal(value.float_v));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            do {
                randomize_buffer(&value.double_v, sizeof(double));
            } while (!isnormal(value.double_v));
            break;
        }

        // FIXME: this one needs a special case
        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            static char new_string[1024];
            int num_words;
            int i;

            new_string[0] = '\0';
            num_words = (rand() % 4) + 1;
            for (i = 0; i < num_words; i ++) {
                strcat(new_string, get_random_word());
                strcat(new_string, " ");
            }
            new_string[strlen(new_string) - 1] = '\0';
            value.string_v = strdup(new_string);
            if (value.string_v == NULL) {
                printf("*** out of memory!\n");
                return;
            }
            break;
        }

        default: {
            printf("*** unknown data type, skipping\n");
            return;
        }
    }

    datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
    if (datapoint == NULL) {
        datapoint = bionet_datapoint_new(resource, &value, NULL);
        bionet_resource_add_existing_datapoint(resource, datapoint);
    } else {
        bionet_datapoint_set_value(datapoint, &value);
        bionet_datapoint_set_timestamp(datapoint, NULL);
    }
}

