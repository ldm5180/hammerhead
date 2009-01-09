
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
    bionet_datapoint_t *datapoint = NULL;
    bionet_value_t *value = NULL;

    switch (bionet_resource_get_data_type(resource)) {
        case BIONET_RESOURCE_DATA_TYPE_INVALID: {
            fprintf(stderr, "*** INVALID DATA TYPE\n");
            return;
        }

        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
	    value = bionet_value_new_binary(resource, rand() % 2);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
	    uint8_t tmp;
            randomize_buffer(&tmp, sizeof(uint8_t));
	    value = bionet_value_new_uint8(resource, tmp);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT8: {
	    int8_t tmp;
            randomize_buffer(&tmp, sizeof(int8_t));
	    value = bionet_value_new_int8(resource, tmp);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
	    uint16_t tmp;
            randomize_buffer(&tmp, sizeof(uint16_t));
	    value = bionet_value_new_uint16(resource, tmp);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT16: {
	    int16_t tmp;
            randomize_buffer(&tmp, sizeof(int16_t));
	    value = bionet_value_new_int16(resource, tmp);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
	    uint32_t tmp;
            randomize_buffer(&tmp, sizeof(uint32_t));
	    value = bionet_value_new_uint32(resource, tmp);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT32: {
	    int32_t tmp;
            randomize_buffer(&tmp, sizeof(int32_t));
	    value = bionet_value_new_int32(resource, tmp);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
	    float tmp;
            do {
                randomize_buffer(&tmp, sizeof(float));
            } while (!isnormal(tmp));
	    value = bionet_value_new_float(resource, tmp);
            break;
        }

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
	    double tmp;
            do {
                randomize_buffer(&tmp, sizeof(double));
            } while (!isnormal(tmp));
	    value = bionet_value_new_double(resource, tmp);
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
	    value = bionet_value_new_str(resource, strdup(new_string)); 
           if (NULL == value) {
                fprintf(stderr, "*** out of memory!\n");
                return;
            }
            break;
        }

        default: {
            fprintf(stderr, "*** unknown data type, skipping\n");
            return;
        }
    }

    datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
    if (datapoint == NULL) {
	datapoint = bionet_datapoint_new(resource, value, NULL);
	bionet_resource_add_datapoint(resource, datapoint);
    } else {
        bionet_datapoint_set_value(datapoint, value);
        bionet_datapoint_set_timestamp(datapoint, NULL);
    }
}

