%inline %{

    /* Object oriented necessities */
#define SWIG_HABOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Hab, 1)
#define SWIG_NODEOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Node, 1)
#define SWIG_RESOURCEOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Resource, 1)
#define SWIG_DATAPOINTOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Datapoint, 1)
#define SWIG_VALUEOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Value, 1)

    Hab * wrap_a_hab(bionet_hab_t * h) {
	if (NULL == h) {
	    g_warning("bionet.i: Unable to wrap NULL bionet_hab_t");
	    return NULL;
	}

	HabUserData * ud = bionet_hab_get_user_data(h);
	if (NULL == ud) {

	    Hab * hab = (Hab *)calloc(1, sizeof(Hab));
	    if (NULL == hab) {
		g_warning("bionet.i: Failed to allocate memory for Hab");
		return NULL;
	    }
	    
	    ud = (HabUserData *)calloc(1, sizeof(HabUserData));
	    if (NULL == ud) {
		g_warning("bionet.i: Failed to allocate memory for Hab User Data");
		free(hab);
		return NULL;
	    }
	    hab->this = h;
	    ud->hab = hab;
	    bionet_hab_set_user_data(hab->this, ud);
	}

	return ud->hab;
    }

    Node * wrap_a_node(bionet_node_t * n) {
	if (NULL == n) {
	    g_warning("bionet.i: Unable to wrap NULL bionet_node_t");
	    return NULL;
	}

	if (NULL == wrap_a_hab(bionet_node_get_hab(n))) {
	    g_warning("bionet.i: Node can't get its hab wrapped.");
	    return NULL;
	}

	Node * node = bionet_node_get_user_data(n);
	if (NULL == node) {
	    node = (Node *)calloc(1, sizeof(Node));
	    if (NULL == node) {
		g_warning("bionet.i: Failed to allocate memory for Node");
		return NULL;
	    }

	    node->this = n;
	    bionet_node_set_user_data(node->this, node);
	}

	int i;
	for (i = 0; i < bionet_node_get_num_resources(n); i++) {
	    bionet_resource_t * res = bionet_node_get_resource_by_index(n, i);
	    if (bionet_resource_get_user_data(res)) {
		continue;
	    }
	    Resource * resource = (Resource *)calloc(1, sizeof(Resource));
	    if (NULL == resource) {
		g_warning("bionet.i: Failed to allocate memory for resource");
		continue;
	    }
	    resource->this = res;
	    bionet_resource_set_user_data(res, resource);
	}

	return node;
    }

    typedef struct {
	void * this;
	int fd;
	PyObject * newHabCallback;
	PyObject * lostHabCallback;
	PyObject * newNodeCallback;
	PyObject * lostNodeCallback;
	PyObject * datapointCallback;
    } Bionet;

    static Bionet * bionet_singleton = NULL;

    void pybionetoo_callback_lost_hab(bionet_hab_t *h) {
	PyObject *arglist;
	PyObject *result = NULL;

	Hab * hab = wrap_a_hab(h);
	if (NULL == hab) {
	    g_warning("bionet.i: Failed to wrap a bionet_hab_t in a Hab");
	    return;
	}

	arglist = Py_BuildValue("(O)", SWIG_HABOO_WRAPPER(hab));
	result = PyEval_CallObject(bionet_singleton->lostHabCallback, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    void pybionetoo_callback_new_hab(bionet_hab_t *h) {
	PyObject *arglist;
	PyObject *result = NULL;

	Hab * hab = wrap_a_hab(h);
	if (NULL == hab) {
	    g_warning("bionet.i: Failed to wrap a bionet_hab_t in a Hab");
	    return;
	}

	arglist = Py_BuildValue("(O)", SWIG_HABOO_WRAPPER(hab));
	result = PyEval_CallObject(bionet_singleton->newHabCallback, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    void pybionetoo_callback_lost_node(bionet_node_t *n) {
	PyObject *arglist;
	PyObject *result = NULL;

	Node * node = wrap_a_node(n);
	if (NULL == node) {
	    g_warning("bionet.i: Failed to wrap a bionet_node_t in a Node");
	    return;
	}

	arglist = Py_BuildValue("(O)", SWIG_NODEOO_WRAPPER(node));
	result = PyEval_CallObject(bionet_singleton->lostNodeCallback, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    void pybionetoo_callback_new_node(bionet_node_t *n) {
	PyObject *arglist;
	PyObject *result = NULL;

	Node * node = wrap_a_node(n);
	if (NULL == node) {
	    g_warning("bionet.i: Failed to wrap a bionet_node_t in a Node");
	    return;
	}

	arglist = Py_BuildValue("(O)", SWIG_NODEOO_WRAPPER(node));
	result = PyEval_CallObject(bionet_singleton->newNodeCallback, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

    void pybionetoo_callback_datapoint(bionet_datapoint_t *d) {
	PyObject *arglist;
	PyObject *result = NULL;
	Datapoint * datapoint;

	/* wrap up the Hab */
	bionet_node_t * n = bionet_datapoint_get_node(d);
	Node * node = wrap_a_node(n);
	if (NULL == node) {
	    g_warning("bionet.i: Failed to wrap a bionet_node_t in a Node");
	    return;
	}

	/* wrap up the datapoint */
	if (NULL == bionet_datapoint_get_user_data(d)) {
	    datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	    if (NULL == datapoint) {
		return;
	    }
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(datapoint->this, datapoint);
	}

	/* wrap up the value */
	bionet_value_t * v = bionet_datapoint_get_value(d);
	if (NULL == bionet_value_get_user_data(v)) {
	    Value * value = (Value *)malloc(sizeof(Value));
	    if (NULL == value) {
		return;
	    }
	    value->this = v;
	    bionet_value_set_user_data(value->this, value);
	}

	arglist = Py_BuildValue("(O)", SWIG_DATAPOINTOO_WRAPPER(datapoint));
	result = PyEval_CallObject(bionet_singleton->datapointCallback, arglist);
	Py_DECREF(arglist);
	if (result == NULL) {
	    return;
	}
	Py_DECREF(result);
    }

%}
