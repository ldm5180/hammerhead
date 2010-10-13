
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


%module bionet
%{
#include "bionet.h"
#include "bionet-util.h"
#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-stream.h"
#include "bionet-resource.h"
#include "bionet-datapoint.h"
#include "bionet-value.h"
#include "bionet-event.h"
#include "bionet-swig-types.h"
%}

%newobject bionet_value_to_str;
%newobject bionet_resource_value_to_str;
%newobject bionet_epsilon_to_str;

%include "libbionet-decl.h"
%include "libbionet-util-decl.h"
%include "bionet.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-stream.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"
%include "bionet-event.h"
%include "bionet-swig-types.h"

%include "cpointer.i"
%pointer_functions(int,      binaryp);
%pointer_functions(uint8_t,  uint8p);
%pointer_functions(int8_t,   int8p);
%pointer_functions(uint16_t, uint16p);
%pointer_functions(int16_t,  int16p);
%pointer_functions(uint32_t, uint32p);
%pointer_functions(int32_t,  int32p);
%pointer_functions(float,  floatp);
%pointer_functions(double,  doublep);
%pointer_functions(char**,  strp);

%include "typemaps.i"
%import "stdint.i"
%import "inttypes.i"

%include "bionet-hab.i"
%include "bionet-node.i"
%include "bionet-resource.i"
%include "bionet-datapoint.i"
%include "bionet-value.i"
%include "bionet-wrappers.i"
%include "bionet-callbacks.i"


%inline %{
#define SWIG_HAB_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_hab_opaque_t, 1)
#define SWIG_NODE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_node_opaque_t, 1)
#define SWIG_RESOURCE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_resource_opaque_t, 1)
#define SWIG_DATAPOINT_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_datapoint_opaque_t, 1)
#define SWIG_STREAM_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_stream_opaque_t, 1)
#define SWIG_VALUE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_value_opaque_t, 1)

    static PyObject * py_cb_lost_hab = NULL;
    static PyObject * py_cb_new_hab = NULL;
    static PyObject * py_cb_lost_node = NULL;
    static PyObject * py_cb_new_node = NULL;
    static PyObject * py_cb_stream = NULL;
    static PyObject * py_cb_datapoint = NULL;
    static GSList * user_data_list;

    void pybionet_callback_lost_hab(bionet_hab_t *hab) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(O)", SWIG_HAB_WRAPPER(hab));
	result = PyEval_CallObject(py_cb_lost_hab, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybionet_register_callback_lost_hab(PyObject * cb) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_lost_hab);  /* Dispose of previous callback */
        py_cb_lost_hab = cb;       /* Remember new callback */

	bionet_register_callback_lost_hab(pybionet_callback_lost_hab);

	return py_cb_lost_hab;
    }


    void pybionet_callback_new_hab(bionet_hab_t *hab) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(O)", SWIG_HAB_WRAPPER(hab));
	result = PyEval_CallObject(py_cb_new_hab, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybionet_register_callback_new_hab(PyObject * cb) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_new_hab);  /* Dispose of previous callback */
        py_cb_new_hab = cb;       /* Remember new callback */
	
	bionet_register_callback_new_hab(pybionet_callback_new_hab);

	return py_cb_new_hab;
    }


    void pybionet_callback_lost_node(bionet_node_t *node) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(O)", SWIG_NODE_WRAPPER(node));
	result = PyEval_CallObject(py_cb_lost_node, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybionet_register_callback_lost_node(PyObject * cb) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_lost_node);  /* Dispose of previous callback */
        py_cb_lost_node = cb;       /* Remember new callback */

	bionet_register_callback_lost_node(pybionet_callback_lost_node);

	return py_cb_lost_node;
    }


    void pybionet_callback_new_node(bionet_node_t *node) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(O)", SWIG_NODE_WRAPPER(node));
	result = PyEval_CallObject(py_cb_new_node, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybionet_register_callback_new_node(PyObject * cb) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_new_node);  /* Dispose of previous callback */
        py_cb_new_node = cb;       /* Remember new callback */

	bionet_register_callback_new_node(pybionet_callback_new_node);

	return py_cb_new_node;
    }

    void pybionet_callback_datapoint(bionet_datapoint_t *datapoint) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(O)", SWIG_DATAPOINT_WRAPPER(datapoint));
	result = PyEval_CallObject(py_cb_datapoint, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybionet_register_callback_datapoint(PyObject * cb) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_datapoint);  /* Dispose of previous callback */
        py_cb_datapoint = cb;       /* Remember new callback */

	bionet_register_callback_datapoint(pybionet_callback_datapoint);

	return py_cb_datapoint;
    }


    void pybionet_callback_stream(bionet_stream_t *stream, void *buffer, int size) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(OOi)", SWIG_STREAM_WRAPPER(stream), buffer, size);
	result = PyEval_CallObject(py_cb_stream, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybionet_register_callback_stream(PyObject * cb) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_stream);  /* Dispose of previous callback */
        py_cb_stream = cb;       /* Remember new callback */

	bionet_register_callback_stream(pybionet_callback_stream);

	return py_cb_stream;
    }

    void pybionet_resource_set_user_data(bionet_resource_t *resource, PyObject *user_data) {

	PyObject * old = bionet_resource_get_user_data(resource);
	if (old == user_data) {
	    return;
	}

	if (NULL != old) {
	    user_data_list = g_slist_remove(user_data_list, old);
	    bionet_resource_set_user_data(resource, NULL);
	    Py_XDECREF(old);
	    Py_XINCREF(user_data);
	    bionet_resource_set_user_data(resource, user_data);
	    user_data_list = g_slist_append(user_data_list, user_data);
	} else {
	    Py_XINCREF(user_data);
	    bionet_resource_set_user_data(resource, user_data);
	    user_data_list = g_slist_append(user_data_list, user_data);
	}
    }

    PyObject * pybionet_resource_get_user_data(bionet_resource_t *resource) {
	return (PyObject *)bionet_resource_get_user_data(resource);
    }

    typedef struct {
	void * this;
    } BionetCache;
%} 

%extend BionetCache {
    BionetCache() {
	BionetCache * cache = (BionetCache *)malloc(sizeof(BionetCache));
	return cache;
    }

    ~BionetCache() {
	free($self);
    }

    unsigned int numHabs() { return bionet_cache_get_num_habs(); }
    
    Hab * hab(unsigned int index) { 
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	if (NULL == hab) {
	    return NULL;
	}

	hab->this = bionet_cache_get_hab_by_index(index);
	if (NULL == hab->this) {
	    free(hab);
	    return NULL;
	}
	
	bionet_hab_set_user_data(hab->this, hab);

	return hab;
    }

    Hab * hab(const char *hab_type, const char *hab_id) { 
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	if (NULL == hab) {
	    return NULL;
	}

	hab->this = bionet_cache_lookup_hab(hab_type, hab_id);
	if (NULL == hab->this) {
	    free(hab);
	    return NULL;
	}

	bionet_hab_set_user_data(hab->this, hab);

	return hab;
    }

    Node * node(const char *hab_type, const char *hab_id, const char *node_id) { 
	Node * node = (Node *)malloc(sizeof(Node));
	if (NULL == node) {
	    return NULL;
	}

	node->this = bionet_cache_lookup_node(hab_type, hab_id, node_id);
	if (NULL == node->this) {
	    free(node);
	    return NULL;
	}

	bionet_node_set_user_data(node->this, node);

	return node;
    }

    Resource * resource(const char *hab_type, const char *hab_id, const char *node_id, const char *resource_id) { 
	Resource * resource = (Resource *)malloc(sizeof(Resource));
	if (NULL == resource) {
	    return NULL;
	}

	resource->this = bionet_cache_lookup_resource(hab_type, hab_id, node_id, resource_id);
	if (NULL == resource->this) {
	    free(resource);
	    return NULL;
	}

	bionet_resource_set_user_data(resource->this, resource);

	return resource;
    }
}


%extend Bionet {
    Bionet() {
	if (bionet_singleton) {
	    return bionet_singleton;
	}
	Bionet * bionet = (Bionet *)malloc(sizeof(Bionet));
	if (NULL == bionet) {
	    return NULL;
	}

	/* Register the callbacks which are currently calling NULL until set from Python */
	bionet_register_callback_new_hab(pybionetoo_callback_new_hab);
	bionet_register_callback_lost_hab(pybionetoo_callback_lost_hab);
	bionet_register_callback_new_node(pybionetoo_callback_new_node);
	bionet_register_callback_lost_node(pybionetoo_callback_lost_node);
	bionet_register_callback_datapoint(pybionetoo_callback_datapoint);

	bionet->fd = bionet_connect();
	if (-1 == bionet->fd) {
	    free(bionet);
	    return NULL;
	}

	bionet_singleton = bionet;
	return bionet_singleton;
    }

    ~Bionet() {
	bionet_disconnect();
	bionet_singleton = NULL;
	free($self);
    }

    int read() {
	return bionet_read();
    }

    int read(struct timeval * tv) {
	return bionet_read_with_timeout(tv);
    }

    int read(struct timeval * tv, unsigned int num) {
	return bionet_read_many(tv, num);
    }

    int subscribe(const char * subscription) {
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
	int r = -1;

	if (bionet_split_name_components_r(subscription,
					   hab_type, hab_id,
					   node_id,
					   resource_id)) {
	    return -1;
	}

	if (0 < strlen(resource_id)) {
	    r = bionet_subscribe_datapoints_by_name(subscription);
	} else if (0 < strlen(node_id)) {
	    r = bionet_subscribe_node_list_by_name(subscription);
	} else if ((0 < strlen(hab_id)) && (0 < strlen(hab_type))) {
	    r = bionet_subscribe_hab_list_by_name(subscription);
	}

	return r;
    }

    int unsubscribe(const char * subscription) {
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
	int r = -1;

	if (bionet_split_name_components_r(subscription,
					   hab_type, hab_id,
					   node_id,
					   resource_id)) {
	    return -1;
	}

	if (0 < strlen(resource_id)) {
	    r = bionet_unsubscribe_datapoints_by_name(subscription);
	} else if (0 < strlen(node_id)) {
	    r = bionet_unsubscribe_node_list_by_name(subscription);
	} else if ((0 < strlen(hab_id)) && (0 < strlen(hab_type))) {
	    r = bionet_unsubscribe_hab_list_by_name(subscription);
	}

	return r;
    }

    int set(Resource * resource, const char * value) {
	return bionet_set_resource(resource->this, value);
    }

    int set(Resource * resource, int value) {
	char valstr[1024];
	if (1024 <= snprintf(valstr, sizeof(valstr), "%d", value)) {
	    return -1;
	}
	return bionet_set_resource(resource->this, valstr);
    }

    int set(Resource * resource, float value) {
	char valstr[1024];
	if (1024 <= snprintf(valstr, sizeof(valstr), "%f", value)) {
	    return -1;
	}
	return bionet_set_resource(resource->this, valstr);
    }

    int set(const char * resource_name, const char * value) {
	return bionet_set_resource_by_name(resource_name, value);
    }

    int set(const char * resource_name, int value) {
	char valstr[1024];
	if (1024 <= snprintf(valstr, sizeof(valstr), "%d", value)) {
	    return -1;
	}
	return bionet_set_resource_by_name(resource_name, valstr);
    }

    int set(const char * resource_name, float value) {
	char valstr[1024];
	if (1024 <= snprintf(valstr, sizeof(valstr), "%f", value)) {
	    return -1;
	}
	return bionet_set_resource_by_name(resource_name, valstr);
    }

    int security(const char * dir, int require) {
	return bionet_init_security(dir, require);
    }
}
