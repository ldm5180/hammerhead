
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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
#include "bionet-asn.h"
#include "bionet-event.h"
#include "bionet-bdm.h"
#include "bionet-swig-types.h"
%}

%newobject bionet_value_to_str;
%newobject bionet_resource_value_to_str;
%newobject bionet_epsilon_to_str;

typedef unsigned short uint16_t;
 
typedef struct timeval
{
	long int tv_sec;
	long int tv_usec;
};

%include "libbionet-asn-decl.h"
%include "libbionet-util-decl.h"
%include "bdm-client.h"
%include "bdm-client-interface.h"
%include "bionet-asn.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"
%include "bionet-event.h"
%include "bionet-bdm.h"
%include "bionet-swig-types.h"

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

%include "bionet-bdm.i"
%include "bionet-hab.i"
%include "bionet-node.i"
%include "bionet-resource.i"
%include "bionet-datapoint.i"
%include "bionet-value.i"
%include "bionet-wrappers.i"
%include "bionet-event.i"
%include "bdm-callbacks.i"
%include "value-operators.i"


%inline %{
#define SWIG_BDM_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_bdm_opaque_t, 1)
#define SWIG_HAB_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_hab_opaque_t, 1)
#define SWIG_NODE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_node_opaque_t, 1)
#define SWIG_RESOURCE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_resource_opaque_t, 1)
#define SWIG_DATAPOINT_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_datapoint_opaque_t, 1)
#define SWIG_VALUE_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_value_opaque_t, 1)
#define SWIG_EVENT_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_bionet_event_opaque_t, 1)

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
    void pybdm_callback_lost_hab(bionet_hab_t *hab, bionet_event_t * event, void * user_data) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(OO)", SWIG_HAB_WRAPPER(hab), SWIG_EVENT_WRAPPER(event));
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


    void pybdm_callback_new_hab(bionet_hab_t *hab, bionet_event_t * event, void * user_data) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(OO)", SWIG_HAB_WRAPPER(hab), SWIG_EVENT_WRAPPER(event));
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


    void pybdm_callback_lost_node(bionet_node_t *node, bionet_event_t * event, void * user_data) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(OO)", SWIG_NODE_WRAPPER(node), SWIG_EVENT_WRAPPER(event));
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


    void pybdm_callback_new_node(bionet_node_t *node, bionet_event_t * event, void * user_data) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(OO)", SWIG_NODE_WRAPPER(node), SWIG_EVENT_WRAPPER(event));
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

    void pybdm_callback_datapoint(bionet_datapoint_t *datapoint, bionet_event_t * event, void * user_data) {
	PyObject *arglist;
	PyObject *result = NULL;

	arglist = Py_BuildValue("(OO)", SWIG_DATAPOINT_WRAPPER(datapoint), SWIG_EVENT_WRAPPER(event));
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

%extend BdmSubscriber {
    BdmSubscriber() {
	if (bdm_subscriber_singleton) {
	    return bdm_subscriber_singleton;
	} 

	BdmSubscriber * bdmsub = (BdmSubscriber *)calloc(1, sizeof(BdmSubscriber));
	if (NULL == bdmsub) {
	    return NULL;
	}
	
	bdm_register_callback_new_bdm(pybdmoo_callback_new_bdm, NULL);
	bdm_register_callback_lost_bdm(pybdmoo_callback_lost_bdm, NULL);
	bdm_register_callback_new_hab(pybdmoo_callback_new_hab, NULL);
	bdm_register_callback_lost_hab(pybdmoo_callback_lost_hab, NULL);
	bdm_register_callback_new_node(pybdmoo_callback_new_node, NULL);
	bdm_register_callback_lost_node(pybdmoo_callback_lost_node, NULL);
	bdm_register_callback_datapoint(pybdmoo_callback_datapoint, NULL);

	bdmsub->fd = bdm_start();
	if (-1 == bdmsub->fd) {
	    free(bdmsub);
	    return NULL;
	}

	bdm_subscriber_singleton = bdmsub;

	return bdmsub;
    }
    
    ~BdmSubscriber() {
	bdm_disconnect();
	bdm_subscriber_singleton = NULL;
	free($self);
    }

    int read() {
	return bdm_read();
    }
    
    int read(struct timeval * timeout) {
	return bdm_read_with_timeout(timeout);
    }

    int read(struct timeval * timeout, unsigned int num) {
	return bdm_read_many(timeout, num);
    }

    int read(unsigned int timeout) {
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	return bdm_read_with_timeout(&tv);
    }

    int read(float timeout) {
	struct timeval tv;
	tv.tv_sec = (int)timeout;
	tv.tv_usec = timeout - (float)tv.tv_sec;
	return bdm_read_with_timeout(&tv);
    }

    int read(unsigned int timeout, unsigned int num) {
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	return bdm_read_many(&tv, num);
    }

    int read(float timeout, unsigned int num) {
	struct timeval tv;
	tv.tv_sec = (int)timeout;
	tv.tv_usec = timeout - (float)tv.tv_sec;
	return bdm_read_many(&tv, num);
    }

    int subscribeToBdm(const char * bdm_name) {
	return bdm_subscribe_bdm_list_by_name(bdm_name);
    }
    
    int subscribeToHab(const char * hab_name) {
	return bdm_subscribe_hab_list_by_name(hab_name);
    }

    int subscribeToNode(const char * node_name) {
	return bdm_subscribe_node_list_by_name(node_name);
    }

    int subscribeToDatapoints(const char * resource_name, struct timeval * start_time, struct timeval * end_time) {
	return bdm_subscribe_datapoints_by_name(resource_name, start_time, end_time);
    }

    int subscribeToDatapoints(const char * resource_name, float start_time, float end_time) {
	struct timeval tv_start;
	struct timeval tv_end;

	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);


	return bdm_subscribe_datapoints_by_name(resource_name, &tv_start, &tv_end);
    }

    int subscribeToDatapoints(const char * resource_name, float start_time, void * Null) {
	struct timeval tv_start;

	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	return bdm_subscribe_datapoints_by_name(resource_name, &tv_start, NULL);
    }

    int subscribeToDatapoints(const char * resource_name, void * Null, float end_time) {
	struct timeval tv_end;

	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);


	return bdm_subscribe_datapoints_by_name(resource_name, NULL, &tv_end);
    }

    int subscribe(const char * resource_name, float start_time, float end_time) {
	struct timeval tv_start;
	struct timeval tv_end;

	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);


	return bdm_subscribe_datapoints_by_name(resource_name, &tv_start, &tv_end);
    }

    int subscribe(const char * resource_name, float start_time, void * Null) {
	struct timeval tv_start;

	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	return bdm_subscribe_datapoints_by_name(resource_name, &tv_start, NULL);
    }

    int subscribe(const char * resource_name, void * Null, float end_time) {
	struct timeval tv_end;

	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);


	return bdm_subscribe_datapoints_by_name(resource_name, NULL, &tv_end);
    }

    int subscribe(const char * resource_name) {
	char peer_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";

	bdm_split_resource_name_r(resource_name,
				  peer_id, bdm_id,
				  hab_type, hab_id,
				  node_id, resource_id);

	if (hab_type[0] && hab_id[0] && node_id[0] && resource_id[0]) {
	    return bdm_subscribe_datapoints_by_name(resource_name, NULL, NULL);
	} else if (hab_type[0] && hab_id[0] && node_id[0]) {
	    return bdm_subscribe_node_list_by_name(resource_name);
	} else if (hab_type[0] && hab_id[0]) {
	    return bdm_subscribe_hab_list_by_name(resource_name);
	}

	return 1;
    }

    int subscribe(const char * resource_name, void *Null1, void *Null2) {
	char peer_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";

	bdm_split_resource_name_r(resource_name,
				  peer_id, bdm_id,
				  hab_type, hab_id,
				  node_id, resource_id);

	if (hab_type[0] && hab_id[0] && node_id[0] && resource_id[0]) {
	    return bdm_subscribe_datapoints_by_name(resource_name, NULL, NULL);
	} else if (hab_type[0] && hab_id[0] && node_id[0]) {
	    return bdm_subscribe_node_list_by_name(resource_name);
	} else if (hab_type[0] && hab_id[0]) {
	    return bdm_subscribe_hab_list_by_name(resource_name);
	}

	return 1;
    }

    int subscribe(const char * resource_name, float start_time, void * Null) {
	char peer_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";

	struct timeval tv_start;
	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	bdm_split_resource_name_r(resource_name,
				  peer_id, bdm_id,
				  hab_type, hab_id,
				  node_id, resource_id);

	if (hab_type[0] && hab_id[0] && node_id[0] && resource_id[0]) {
	    return bdm_subscribe_datapoints_by_name(resource_name, &tv_start, NULL);
	} else if (hab_type[0] && hab_id[0] && node_id[0]) {
	    return bdm_subscribe_node_list_by_name(resource_name);
	} else if (hab_type[0] && hab_id[0]) {
	    return bdm_subscribe_hab_list_by_name(resource_name);
	}

	return 1;
    }

    int subscribe(const char * resource_name, void *Null, float end_time) {
	char peer_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";

	struct timeval tv_end;
	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);

	bdm_split_resource_name_r(resource_name,
				  peer_id, bdm_id,
				  hab_type, hab_id,
				  node_id, resource_id);

	if (hab_type[0] && hab_id[0] && node_id[0] && resource_id[0]) {
	    return bdm_subscribe_datapoints_by_name(resource_name, &tv_end, NULL);
	} else if (hab_type[0] && hab_id[0] && node_id[0]) {
	    return bdm_subscribe_node_list_by_name(resource_name);
	} else if (hab_type[0] && hab_id[0]) {
	    return bdm_subscribe_hab_list_by_name(resource_name);
	}

	return 1;
    }

    int subscribe(const char * resource_name, float start_time, float end_time) {
	char peer_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";

	struct timeval tv_start;
	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	struct timeval tv_end;
	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);

	bdm_split_resource_name_r(resource_name,
				  peer_id, bdm_id,
				  hab_type, hab_id,
				  node_id, resource_id);

	if (hab_type[0] && hab_id[0] && node_id[0] && resource_id[0]) {
	    return bdm_subscribe_datapoints_by_name(resource_name, &tv_start, &tv_end);
	} else if (hab_type[0] && hab_id[0] && node_id[0]) {
	    return bdm_subscribe_node_list_by_name(resource_name);
	} else if (hab_type[0] && hab_id[0]) {
	    return bdm_subscribe_hab_list_by_name(resource_name);
	}

	return 1;
    }

    int unsubscribe(const char *resource_name) {
	return bdm_unsubscribe_datapoints_by_name(resource_name, NULL, NULL);
    }

    int unsubscribe(const char *resource_name, void *Null1, void *Null2) {
	return bdm_unsubscribe_datapoints_by_name(resource_name, NULL, NULL);
    }

    int unsubscribe(const char *resource_name, float start_time, void * Null) {
	struct timeval tv_start;

	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	return bdm_unsubscribe_datapoints_by_name(resource_name, &tv_start, NULL);
    }

    int unsubscribe(const char *resource_name, void *Null, float end_time) {
	struct timeval tv_end;

	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);

	return bdm_unsubscribe_datapoints_by_name(resource_name, NULL, &tv_end);
    }

    int unsubscribe(const char *resource_name, float start_time, float end_time) {
	struct timeval tv_start;
	struct timeval tv_end;

	tv_start.tv_sec = (time_t)start_time;
	tv_start.tv_usec = (suseconds_t)((start_time - (float)tv_start.tv_sec) * 1000000);

	tv_end.tv_sec = (time_t)end_time;
	tv_end.tv_usec = (suseconds_t)((end_time - (float)tv_end.tv_sec) * 1000000);

	return bdm_unsubscribe_datapoints_by_name(resource_name, &tv_start, &tv_end);
    }

    unsigned int numBdms() {
	return bdm_cache_get_num_bdms();
    }

    Bdm * bdm(unsigned int index) {
	Bdm * bdm = NULL;
	bionet_bdm_t * b = bdm_cache_get_bdm_by_index(index);
	if (NULL == b) {
	    return NULL;
	}
	
	bdm = bionet_bdm_get_user_data(b);

	if (NULL == bdm) {
	    bdm = (Bdm *)calloc(1, sizeof(Bdm));
	    if (NULL == bdm) {
		return NULL;
	    }
	    bdm->this = b;
	    bionet_bdm_set_user_data(bdm->this, bdm);
	}

	return bdm;
    }

    Bdm * bdm(const char * id) {
	Bdm * bdm = NULL;
	bionet_bdm_t * b = bdm_cache_lookup_bdm(id);
	if (NULL == b) {
	    return NULL;
	}
	
	bdm = bionet_bdm_get_user_data(b);

	if (NULL == bdm) {
	    bdm = (Bdm *)calloc(1, sizeof(Bdm));
	    if (NULL == bdm) {
		return NULL;
	    }
	    bdm->this = b;
	    bionet_bdm_set_user_data(bdm->this, bdm);
	}

	return bdm;	
    }

    unsigned int numHabs() {
	return bdm_cache_get_num_habs();
    }

    Hab * hab(unsigned int index) {
	Hab * hab = NULL;
	bionet_hab_t * h = bdm_cache_get_hab_by_index(index);
	if (NULL == h) {
	    return NULL;
	}
	
	hab = bionet_hab_get_user_data(h);

	if (NULL == hab) {
	    hab = (Hab *)calloc(1, sizeof(Hab));
	    if (NULL == hab) {
		return NULL;
	    }
	    hab->this = h;
	    bionet_hab_set_user_data(hab->this, hab);
	}

	return hab;
    }

    Hab * hab(const char * type, const char * id) {
	Hab * hab = NULL;
	bionet_hab_t * h = bdm_cache_lookup_hab(type, id);
	if (NULL == h) {
	    return NULL;
	}
	
	hab = bionet_hab_get_user_data(h);

	if (NULL == hab) {
	    hab = (Hab *)calloc(1, sizeof(Hab));
	    if (NULL == hab) {
		return NULL;
	    }
	    hab->this = h;
	    bionet_hab_set_user_data(hab->this, hab);
	}

	return hab;	
    }

    Hab * hab(const char * hab_name) {
	Hab * hab = NULL;

	/* split the name */
	char type[BIONET_NAME_COMPONENT_MAX_LEN];
	char id[BIONET_NAME_COMPONENT_MAX_LEN];
	if (bionet_split_hab_name_r(hab_name, type, id)) {
	    return NULL;
	}

	bionet_hab_t * h = bdm_cache_lookup_hab(type, id);
	if (NULL == h) {
	    return NULL;
	}
	
	hab = bionet_hab_get_user_data(h);

	if (NULL == hab) {
	    hab = (Hab *)calloc(1, sizeof(Hab));
	    if (NULL == hab) {
		return NULL;
	    }
	    hab->this = h;
	    bionet_hab_set_user_data(hab->this, hab);
	}

	return hab;	
    }

    Node * node(const char *hab_type, const char *hab_id, const char *node_id) {
	Node * node = NULL;
	bionet_node_t * n = bdm_cache_lookup_node(hab_type, hab_id, node_id);
	if (NULL == n) {
	    return NULL;
	}
	
	node = bionet_node_get_user_data(n);

	if (NULL == node) {
	    node = (Node *)calloc(1, sizeof(Node));
	    if (NULL == node) {
		return NULL;
	    }
	    node->this = n;
	    bionet_node_set_user_data(node->this, node);
	}

	return node;	
    }

    Node * node(const char *node_name) {
	Node * node = NULL;
	
	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";

	bionet_split_node_name_r(node_name,
				 hab_type, hab_id,
				 node_id);

	bionet_node_t * n = bdm_cache_lookup_node(hab_type, hab_id, node_id);
	if (NULL == n) {
	    return NULL;
	}
	
	node = bionet_node_get_user_data(n);

	if (NULL == node) {
	    node = (Node *)calloc(1, sizeof(Node));
	    if (NULL == node) {
		return NULL;
	    }
	    node->this = n;
	    bionet_node_set_user_data(node->this, node);
	}

	return node;	
     }

    Resource * resource(const char *hab_type, const char *hab_id, const char *node_id, const char * resource_id) {
	Resource * resource = NULL;
	bionet_resource_t * r = bdm_cache_lookup_resource(hab_type, hab_id, node_id, resource_id);
	if (NULL == r) {
	    return NULL;
	}
	
	resource = bionet_resource_get_user_data(r);

	if (NULL == resource) {
	    resource = (Resource *)calloc(1, sizeof(Resource));
	    if (NULL == resource) {
		return NULL;
	    }
	    resource->this = r;
	    bionet_resource_set_user_data(resource->this, resource);
	}

	return resource;	
    }

    Resource * resource(const char *resource_name) {
	Resource * resource = NULL;

	char hab_type[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char hab_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char node_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";
	char resource_id[BIONET_NAME_COMPONENT_MAX_LEN] = "\0";

	bionet_split_resource_name_r(resource_name,
				     hab_type, hab_id,
				     node_id, resource_id);

	bionet_resource_t * r = bdm_cache_lookup_resource(hab_type, hab_id, node_id, resource_id);
	if (NULL == r) {
	    return NULL;
	}
	
	resource = bionet_resource_get_user_data(r);

	if (NULL == resource) {
	    resource = (Resource *)calloc(1, sizeof(Resource));
	    if (NULL == resource) {
		return NULL;
	    }
	    resource->this = r;
	    bionet_resource_set_user_data(resource->this, resource);
	}

	return resource;	
    }

}
