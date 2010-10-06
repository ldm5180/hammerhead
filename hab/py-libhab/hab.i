
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


%module hab
%{
#include "hardware-abstractor.h"
#include "bionet-util.h"
#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-resource.h"
#include "bionet-datapoint.h"
#include "bionet-value.h"
#include "bionet-epsilon.h"
#include "bionet-swig-types.h"
%}

%newobject bionet_value_to_str;
%newobject bionet_resource_value_to_str;
%newobject bionet_epsilon_to_str;

typedef struct timeval
{
    long int tv_sec;
    long int tv_usec;
};

%include "libhab-decl.h"
%include "libbionet-util-decl.h"
%include "hardware-abstractor.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"
%include "bionet-epsilon.h"
%include "bionet-swig-types.h"

%include "typemaps.i"
%import "stdint.i"
%import "inttypes.i"

%include "bionet-hab.i"
%include "bionet-node.i"
%include "bionet-resource.i"
%include "bionet-datapoint.i"
%include "bionet-value.i"

%inline %{
#define SWIG_RESOURCE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_resource_opaque_t, 1)
#define SWIG_VALUE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_value_opaque_t, 1)

    static PyObject * py_set_resource_callback = NULL;

    void python_callback(bionet_resource_t *resource, bionet_value_t *value)
    {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(OO)", SWIG_RESOURCE_WRAPPER(resource), SWIG_VALUE_WRAPPER(value));
	result = PyEval_CallObject(py_set_resource_callback, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pyhab_register_callback_set_resource(PyObject * cb)
    {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_set_resource_callback);  /* Dispose of previous callback */
        py_set_resource_callback = cb;       /* Remember new callback */

	hab_register_callback_set_resource(python_callback);

	return py_set_resource_callback;
    }


#define SWIG_OORESOURCE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Resource, 1)
#define SWIG_OOVALUE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Value, 1)

    void pythonoo_set_resource_callback(bionet_resource_t *resource, bionet_value_t *value)
    {
	PyObject *arglist;
	PyObject *result = NULL;
	
	bionet_hab_t * h = bionet_resource_get_hab(resource);
	Hab * hab = bionet_hab_get_user_data(h);
	
	Resource * r = (Resource *)bionet_resource_get_user_data(resource);
	Value * v = (Value *)malloc(sizeof(Value));
	if (NULL == v) {
	    g_warning("pythonoo_callback: Failed to allocate Value.");
	    return;
	}
	
	v->this = value;
	bionet_value_set_user_data(value, v);
	
	arglist = Py_BuildValue("(OO)", SWIG_OORESOURCE_WRAPPER(r), SWIG_OOVALUE_WRAPPER(v));
	
	result = PyEval_CallObject(hab->set_resource_callback, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

%}

%extend Hab {
    int connect() {
	$self->fd = hab_connect($self->this);
	return $self->fd;
    }

    void disconnect() {
	hab_disconnect();
    }

    int reportNode(Node * node) {
	if (NULL == node) {
	    return hab_report_new_node(NULL);
	}
	return hab_report_new_node(node->this);
    }

    int reportDatapoints(Node * node) {
	if (NULL == node) {
	    return hab_report_datapoints(NULL);
	}
	return hab_report_datapoints(node->this);	
    }

    int reportLostNode(Node * node) {
	if (NULL == node) {
	    return hab_report_lost_node(NULL);
	}
	return hab_report_lost_node(node->this);
    }

    void read() {
	hab_read();
    }

    void read(struct timeval * timeout) {
	hab_read_with_timeout(timeout);
    }

    PyObject * registerSetResourceCallback(PyObject * cb) {
	if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
	if (NULL != $self->set_resource_callback)
	    Py_XDECREF($self->set_resource_callback);  /* Dispose of previous callback */
        $self->set_resource_callback = cb;       /* Remember new callback */

	hab_register_callback_set_resource(pythonoo_set_resource_callback);

	return $self->set_resource_callback;
    }
}
