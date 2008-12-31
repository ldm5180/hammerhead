
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef INTERNAL_H
#define INTERNAL_H

#include "bionet-util-2.1.h"

struct bionet_hab_opaque_t {
    char *type;
    char *id;

    GSList *nodes;

    void *user_data;
};


struct bionet_node_opaque_t {
    bionet_hab_t *hab;

    char *id;

    GSList *resources;
    GSList *streams;

    void *user_data;
};


struct bionet_stream_opaque_t {
    bionet_node_t *node;

    // this describes the stream
    char *id;
    bionet_stream_direction_t direction;
    char *type;

    void *user_data;
};


/**
 * @union bionet_datapoint_value_t
 * @brief Union to describe how to read a value 
 *
 * @note bionet_datapoint_value_t should always be initialized 
 * (all-bits-zero is valid)
 */
typedef union {
    int binary_v; /**< 0 or 1, TRUE or FALSE */

    uint8_t  uint8_v; /**< 8-bit unsigned integer */
    int8_t   int8_v; /**< 8-bit signed integer */

    uint16_t uint16_v; /**< 16-bit unsigned integer */
    int16_t  int16_v; /**< 16-bit signed integer */

    uint32_t uint32_v; /**< 32-bit unsigned integer */
    int32_t  int32_v; /**< 32-bit signed integer */

    float float_v; /**< float */
    double double_v; /**< double */

    char *string_v; /**< NULL-terminated array of characters */
} bionet_datapoint_value_t;


// 
// This holds a resource.  'flavor', 'id', and 'data_type' are all used by
// the system.  'user_data' is for the caller to do with as it pleases.
//
struct bionet_resource_opaque_t {
    bionet_node_t *node;

    char *id;

    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;

    GPtrArray *datapoints;

    void *user_data;
};

struct bionet_value_opaque_t {
    bionet_resource_t * resource;
    bionet_datapoint_value_t content;
};


struct bionet_datapoint_opaque_t {
    bionet_value_t * value;
    struct timeval timestamp;

    int dirty;  // 1 if the datapoint has new information that hasnt been reported to Bionet, 0 if the datapoint has nothing new
};


#endif /* INTERNAL_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
