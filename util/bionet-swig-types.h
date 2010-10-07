
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

#endif /* BIONET_SWIG_TYPES */
