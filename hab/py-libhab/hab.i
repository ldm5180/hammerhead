
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
%}

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

%include "typemaps.i"
%import "stdint.i"
%import "inttypes.i"


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
%}

