%inline %{

    /* Object oriented necessities */
#define SWIG_HABOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Hab, 1)
#define SWIG_NODEOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Node, 1)
#define SWIG_RESOURCEOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Resource, 1)
#define SWIG_DATAPOINTOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Datapoint, 1)
#define SWIG_VALUEOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Value, 1)
#define SWIG_BDMOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Bdm, 1)
#define SWIG_EVENTOO_WRAPPER(name) SWIG_NewPointerObj((void*)name, SWIGTYPE_p_Event, 1)

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

	bionet_hab_increment_ref_count(h);
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
		g_warning("bionet-wrappers.i: Failed to allocate memory for resource");
		continue;
	    }
	    resource->this = res;
	    bionet_resource_set_user_data(res, resource);
	}

	bionet_node_increment_ref_count(n);
	return node;
    }

    Bdm * wrap_a_bdm(bionet_bdm_t * b) {
	if (NULL == b) {
	    g_warning("bionet-wrappers.i: Unable to wrap NULL bionet_bdm_t");
	    return NULL;
	}

	Bdm * bdm = bionet_bdm_get_user_data(b);
	if (NULL == bdm) {

	    bdm = (Bdm *)calloc(1, sizeof(Bdm));
	    if (NULL == bdm) {
		g_warning("bionet-wrappers.i: Failed to allocate memory for Bdm");
		return NULL;
	    }
	    
	    bdm->this = b;
	    bionet_bdm_set_user_data(bdm->this, bdm);
	}

	bionet_bdm_increment_ref_count(b);
	return bdm;
    }

    Event * wrap_a_event(bionet_event_t * e) {
	if (NULL == e) {
	    g_warning("bionet-wrappers.i: Unable to wrap NULL bionet_event_t");
	    return NULL;
	}

	Event * event = bionet_event_get_user_data(e);
	if (NULL == event) {

	    event = (Event *)calloc(1, sizeof(Event));
	    if (NULL == event) {
		g_warning("bionet-wrappers.i: Failed to allocate memory for Event");
		return NULL;
	    }
	    
	    event->this = e;
	    bionet_event_set_user_data(event->this, event);
	}

	bionet_event_increment_ref_count(e);
	return event;      
    }

%}
