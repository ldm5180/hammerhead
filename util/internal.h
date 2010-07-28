
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef INTERNAL_H
#define INTERNAL_H

#include "bionet-util.h"

#include <stdint.h>



struct bionet_bdm_opaque_t {
    char *id;

    GSList *habs;

    int peer_refcount; // Number of peers that point to this bdm

    const void *user_data;
};

struct bionet_hab_opaque_t {
    char *type;
    char *id;

    char *name;

    GSList *nodes;

    const void *user_data;

    int is_secure;

    GSList *bdms; // NULL unless in bdm library
    char * recording_bdm; //NULL unless in bdm library
};


struct bionet_node_opaque_t {
    bionet_hab_t *hab;

    char *id;

    char *name;

    GSList *resources;
    GSList *streams;

    const void *user_data;  // const because the bionet library wont monkey with it
};


struct bionet_stream_opaque_t {
    bionet_node_t *node;

    // this describes the stream
    char *id;
    bionet_stream_direction_t direction;
    char *type;

    char *name;
    char *local_name;

    const void *user_data;  // const because the bionet library wont monkey with it
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

    char *name;
    char *local_name;

    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;

    GPtrArray *datapoints;

    const void *user_data;

    int persist;
};

struct bionet_value_opaque_t {
    bionet_resource_t * resource;
    bionet_datapoint_value_t content;

    char * as_string;
};


struct bionet_datapoint_opaque_t {
    bionet_value_t * value;
    struct timeval timestamp;

    int dirty;  // 1 if the datapoint has new information that hasnt been reported to Bionet, 0 if the datapoint has nothing new
};

/**
 * @brief Parse out the parameters from the querystring
 *
 * Querystring is very close to a URI querystring
 *
 * The params are all strings, and can be converted with bionet_param_to_timeval,
 * bionet_param_to_int, etc...
 *
 * @param[in] querystring
 *   The string that has the parameters encoded
 *
 * @param[out] ret_params
 *   Points to a newly alloceaded GHAshTable containing the parameters decoded
 *   from the query string
 *
 * @retval 0 Parsed successfully, ret_params alloced
 * @retval -1 Parse error. ret_params not allocated
 */
int bionet_parse_topic_params(
        const char * querystring,
        GHashTable ** ret_params);


/**
 * @brief Convert the string to a timeval
 *
 * Convert the string which was decoded from a topic query string into a timeval struct
 *
 * @param params
 *   Hash table created with bionet_parse_topic_params
 *
 * @param key
 *   The param name to convert
 *
 * @param tv
 *   Points to a timeval struct that will be populated with the result
 *
 * @retval -1 THe parameter doesn't exist, or conversion error
 * @retval 0 The tv struct ahs been populated with the value extracted from the string
 */
int bionet_param_to_timeval(GHashTable * params, const char * key, struct timeval * tv);

#endif /* INTERNAL_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
