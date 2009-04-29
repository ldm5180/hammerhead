
// Copyright (c) 2008-2009, Regents of the University of Colorado.
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
%}

%include "bionet.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-stream.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"

%include "typemaps.i"
%import "stdint.i"
%import "inttypes.i"

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

%} 
