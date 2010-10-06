
#ifndef BIONET_SWIG_TYPES
#define BIONET_SWIG_TYPES

typedef struct {
    bionet_hab_t * this;
    int fd;
    PyObject * set_resource_callback;
} Hab;

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

#ifdef LIBHAB
void pythonoo_set_resource_callback(bionet_resource_t *resource, bionet_value_t *value);
#endif /*LIBHAB */

#endif /* BIONET_SWIG_TYPES */
