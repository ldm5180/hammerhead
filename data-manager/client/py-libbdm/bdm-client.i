
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


%module bdm_client
%{
#include "bdm-client.h"
#include "bdm-client-interface.h"
#include "bionet-util.h"
#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-resource.h"
#include "bionet-datapoint.h"
#include "bionet-value.h"
%}

typedef unsigned short uint16_t;
 
typedef struct timeval
{
	long int tv_sec;
	long int tv_usec;
};

%include "bdm-client.h"
%include "bdm-client-interface.h"
%include "bionet-asn.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"

%include "cpointer.i"
%pointer_functions(int,      binaryp);
%pointer_functions(uint8_t,  uint8p);
%pointer_functions(int8_t,   int8p);
%pointer_functions(uint16_t, uint16p);
%pointer_functions(int16_t,  int16p);
%pointer_functions(uint32_t, uint32p);
%pointer_functions(int32_t,  int32p);
%pointer_functions(char**,  strp);

%include "typemaps.i"
%import "stdint.i"
%import "inttypes.i"

%inline %{
#define SWIG_BDM_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_bdm_opaque_t, 1)
#define SWIG_HAB_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_hab_opaque_t, 1)
#define SWIG_NODE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_node_opaque_t, 1)
#define SWIG_RESOURCE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_resource_opaque_t, 1)
#define SWIG_DATAPOINT_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_datapoint_opaque_t, 1)
#define SWIG_VALUE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_value_opaque_t, 1)

    /* callbacks */
    static PyObject * py_cb_lost_bdm = NULL;
    static PyObject * py_cb_new_bdm = NULL;
    static PyObject * py_cb_lost_hab = NULL;
    static PyObject * py_cb_new_hab = NULL;
    static PyObject * py_cb_lost_node = NULL;
    static PyObject * py_cb_new_node = NULL;
    static PyObject * py_cb_datapoint = NULL;

    /* user data */
    static PyObject * py_cb_lost_bdm_user_data = NULL;
    static PyObject * py_cb_new_bdm_user_data = NULL;
    static PyObject * py_cb_lost_hab_user_data = NULL;
    static PyObject * py_cb_new_hab_user_data = NULL;
    static PyObject * py_cb_lost_node_user_data = NULL;
    static PyObject * py_cb_new_node_user_data = NULL;
    static PyObject * py_cb_datapoint_user_data = NULL;


    void pybdm_callback_lost_bdm(bionet_bdm_t *bdm, void * user_data) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(O)", SWIG_BDM_WRAPPER(bdm));
	result = PyEval_CallObject(py_cb_lost_bdm, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybdm_register_callback_lost_bdm(PyObject * cb, PyObject * user_data) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }

        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_lost_bdm);  /* Dispose of previous callback */
        py_cb_lost_bdm = cb;       /* Remember new callback */

        Py_XINCREF(user_data);  /* Add a reference to new user data */	
	Py_XDECREF(py_cb_lost_bdm_user_data);  
        py_cb_lost_bdm_user_data = user_data;       /* Remember new user data */

	bdm_register_callback_lost_bdm(pybdm_callback_lost_bdm, py_cb_lost_bdm_user_data);

	return py_cb_lost_bdm;
    }


    void pybdm_callback_new_bdm(bionet_bdm_t *bdm, void * user_data) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(O)", SWIG_BDM_WRAPPER(bdm));
	result = PyEval_CallObject(py_cb_new_bdm, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    PyObject * pybdm_register_callback_new_bdm(PyObject * cb, PyObject * user_data) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_new_bdm);  /* Dispose of previous callback */
        py_cb_new_bdm = cb;       /* Remember new callback */
	
        Py_XINCREF(user_data);  /* Add a reference to new user data */	
	Py_XDECREF(py_cb_new_bdm_user_data);  
        py_cb_new_bdm_user_data = user_data;       /* Remember new user data */

	bdm_register_callback_new_bdm(pybdm_callback_new_bdm, py_cb_new_bdm_user_data);

	return py_cb_new_bdm;
    }


    /* HABs */
    void pybdm_callback_lost_hab(bionet_hab_t *hab, void * user_data) {
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

    PyObject * pybdm_register_callback_lost_hab(PyObject * cb, PyObject * user_data) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_lost_hab);  /* Dispose of previous callback */
        py_cb_lost_hab = cb;       /* Remember new callback */

        Py_XINCREF(user_data);  /* Add a reference to new user data */	
	Py_XDECREF(py_cb_lost_hab_user_data);  
        py_cb_lost_hab_user_data = user_data;       /* Remember new user data */

	bdm_register_callback_lost_hab(pybdm_callback_lost_hab, py_cb_lost_hab_user_data);

	return py_cb_lost_hab;
    }


    void pybdm_callback_new_hab(bionet_hab_t *hab, void * user_data) {
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

    PyObject * pybdm_register_callback_new_hab(PyObject * cb, PyObject * user_data) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_new_hab);  /* Dispose of previous callback */
        py_cb_new_hab = cb;       /* Remember new callback */
	
        Py_XINCREF(user_data);  /* Add a reference to new user data */	
	Py_XDECREF(py_cb_new_hab_user_data);  
        py_cb_new_hab_user_data = user_data;       /* Remember new user data */

	bdm_register_callback_new_hab(pybdm_callback_new_hab, py_cb_new_hab_user_data);

	return py_cb_new_hab;
    }


    void pybdm_callback_lost_node(bionet_node_t *node, void * user_data) {
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

    PyObject * pybdm_register_callback_lost_node(PyObject * cb, PyObject * user_data) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_lost_node);  /* Dispose of previous callback */
        py_cb_lost_node = cb;       /* Remember new callback */

        Py_XINCREF(user_data);  /* Add a reference to new user data */	
	Py_XDECREF(py_cb_lost_node_user_data);  
        py_cb_lost_node_user_data = user_data;       /* Remember new user data */

	bdm_register_callback_lost_node(pybdm_callback_lost_node, py_cb_lost_node_user_data);

	return py_cb_lost_node;
    }


    void pybdm_callback_new_node(bionet_node_t *node, void * user_data) {
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

    PyObject * pybdm_register_callback_new_node(PyObject * cb, PyObject * user_data) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_new_node);  /* Dispose of previous callback */
        py_cb_new_node = cb;       /* Remember new callback */

        Py_XINCREF(user_data);  /* Add a reference to new user data */	
	Py_XDECREF(py_cb_new_node_user_data);  
        py_cb_new_node_user_data = user_data;       /* Remember new user data */

	bdm_register_callback_new_node(pybdm_callback_new_node, py_cb_new_node_user_data);

	return py_cb_new_node;
    }

    void pybdm_callback_datapoint(bionet_datapoint_t *datapoint, void * user_data) {
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

    PyObject * pybdm_register_callback_datapoint(PyObject * cb, PyObject * user_data) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(cb);         /* Add a reference to new callback */
        Py_XDECREF(py_cb_datapoint);  /* Dispose of previous callback */
        py_cb_datapoint = cb;       /* Remember new callback */

        Py_XINCREF(user_data);  /* Add a reference to new user data */	
	Py_XDECREF(py_cb_datapoint_user_data);  
        py_cb_datapoint_user_data = user_data;       /* Remember new user data */

	bdm_register_callback_datapoint(pybdm_callback_datapoint, py_cb_datapoint_user_data);

	return py_cb_datapoint;
    }

%} 
