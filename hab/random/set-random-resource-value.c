
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#define _ISOC99_SOURCE  // for isnormal()

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

    datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
    if (datapoint == NULL) {
        datapoint = bionet_datapoint_new(resource, "0", NULL);
        bionet_resource_add_existing_datapoint(resource, datapoint);
    } else {
        bionet_datapoint_set_timestamp(datapoint, NULL);
    }

    switch (resource->data_type) {
        case BIONET_RESOURCE_DATA_TYPE_INVALID: {
            printf("*** INVALID DATA TYPE\n");
            return;
        }

        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            datapoint->value.binary_v = rand() % 2;
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            randomize_buffer(&datapoint->value.uint8_v, sizeof(uint8_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            randomize_buffer(&datapoint->value.int8_v, sizeof(int8_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            randomize_buffer(&datapoint->value.uint16_v, sizeof(uint16_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            randomize_buffer(&datapoint->value.int16_v, sizeof(int16_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            randomize_buffer(&datapoint->value.uint32_v, sizeof(uint32_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            randomize_buffer(&datapoint->value.int32_v, sizeof(int32_t));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            do {
                randomize_buffer(&datapoint->value.float_v, sizeof(float));
            } while (!isnormal(datapoint->value.float_v));
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            do {
                randomize_buffer(&datapoint->value.double_v, sizeof(double));
            } while (!isnormal(datapoint->value.double_v));
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
            bionet_datapoint_value_from_string(datapoint, new_string);
            return;
        }

        default: {
            printf("*** unknown data type, skipping\n");
            return;
        }
    }
}

