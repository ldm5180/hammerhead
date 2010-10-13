%inline %{
    typedef struct {
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
	datapoint = bionet_datapoint_get_user_data(d);
	if (NULL == datapoint) {
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
