
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef __BIONET_DATAPOINT_H
#define __BIONET_DATAPOINT_H




#include <stdint.h>
#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"




//
//  bionet_datapoint_value_t should always be initialized (zero is valid)
//

typedef union {
    int binary_v;

    uint8_t  uint8_v;
    int8_t   int8_v;

    uint16_t uint16_v;
    int16_t  int16_v;

    uint32_t uint32_v;
    int32_t  int32_v;

    float float_v;
    double double_v;

    char *string_v;
} bionet_datapoint_value_t;


struct bionet_datapoint {
    bionet_resource_t *resource;  // the resource that this datapoint belongs to

    bionet_datapoint_value_t value;
    struct timeval timestamp;
};




//
//
//       NAME:  bionet_datapoint_new()
//
//   FUNCTION:  Allocates and initializes a new datapoint.  Does *not* add
//              it to the Resource's list of data points.
//
//  ARGUMENTS:  All passed-in strings are considered the property of the
//              caller.  The caller is free to overwrite or free the
//              strings on return from this function.
//
//              resource: The Resource that the new datapoint is for.
//
//              value: The value of the new datapoint, as an ASCII string.
//
//              timestamp: The timestamp of the new datapoint.
//
//    RETURNS:  The new Datapoint if all went well, NULL on error.
//
//

bionet_datapoint_t *bionet_datapoint_new(
    bionet_resource_t *resource,
    const char *value,
    const struct timeval *timestamp
);




int bionet_datapoint_value_from_string(bionet_datapoint_t *d, const char *value_string);
int bionet_datapoint_value_from_string_isolated(bionet_resource_data_type_t data_type, bionet_datapoint_value_t *value, const char *value_string);




//
//       NAME:  bionet_datapoint_value_to_string()
//
//   FUNCTION:  Renders a Datapoint's Value as an ASCII string.
//
//  ARGUMENTS:  The Datapoint to get the Value from.
//
//    RETURNS:  A statically allocated string containing an ASCII
//              representation of the Datapoint Value, or NULL on error.
//

const char *bionet_datapoint_value_to_string(const bionet_datapoint_t *datapoint);
const char *bionet_datapoint_value_to_string_isolated(bionet_resource_data_type_t data_type, const bionet_datapoint_value_t *value);




//
//       NAME:  bionet_datapoint_timestamp_to_string()
//
//   FUNCTION:  Renders the Datapoint Timestamp as an ASCII string of the
//              form "YYYY-MM-DD HH:MM:SS.microseconds".
//
//  ARGUMENTS:  The Datapoint to get the Timestamp of.
//
//    RETURNS:  A pointer to the statically allocated string on success,
//              NULL on failure.
//

const char *bionet_datapoint_timestamp_to_string(const bionet_datapoint_t *datapoint);




//
//       NAME:  bionet_datapoint_free()
//
//   FUNCTION:  Frees a Datapoint created with bionet_datapoint_new().
//
//  ARGUMENTS:  The Datapoint to free.
//
//    RETURNS:  Nothing.
//
//

void bionet_datapoint_free(bionet_datapoint_t *datapoint);




#endif // __BIONET_DATAPOINT_H


