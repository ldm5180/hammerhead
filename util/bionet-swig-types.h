
#ifndef BIONET_SWIG_TYPES
#define BIONET_SWIG_TYPES

typedef struct {
    bionet_hab_t * this;
} Hab;

typedef struct {
    void * hab;
    void * hp;
} HabUserData;

typedef struct {
    bionet_node_t * this;
} Node;

typedef struct {
    bionet_resource_t * this;
} Resource;

typedef struct {
    bionet_datapoint_t * this;
} Datapoint;

typedef struct {
    bionet_value_t * this;
} Value;

typedef struct {
    bionet_bdm_t * this;
} Bdm;

typedef struct {
    bionet_epsilon_t * this;
    bionet_resource_data_type_t datatype;
} Epsilon;

typedef struct {
    bionet_stream_t * this;
} Stream;

#endif /* BIONET_SWIG_TYPES */
