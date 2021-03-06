%extend Hab {
    Hab(const char * type, const char * id) {
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	if (NULL == hab) {
	    return hab;
	}
	
	HabUserData * ud = (HabUserData *)malloc(sizeof(HabUserData));
	if (NULL == ud) {
	    free(hab);
	    return NULL;
	}
	
	hab->this = bionet_hab_new(type, id);
	if (NULL == hab->this) {
	    free(hab);
	    return NULL;
	}

	ud->hab = hab;
	ud->hp = NULL;
	bionet_hab_set_user_data(hab->this, ud);

	return hab;
    }
    Hab() {
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	if (NULL == hab) {
	    return hab;
	}

	HabUserData * ud = (HabUserData *)malloc(sizeof(HabUserData));
	if (NULL == ud) {
	    free(hab);
	    return NULL;
	}
	
	hab->this = bionet_hab_new(NULL, NULL);
	if (NULL == hab->this) {
	    free(hab);
	    return NULL;
	}

	ud->hab = hab;
	ud->hp = NULL;
	bionet_hab_set_user_data(hab->this, ud);

	return hab;
    }
    ~Hab() {
	int free_me = 0;
	if (0 == bionet_hab_get_ref_count($self->this)) {
	    bionet_hab_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_hab_free($self->this);
	if (free_me) {
	    free($self);
	}
    }
    const char * name() {
	return bionet_hab_get_name((bionet_hab_t *)$self->this);
    }
    const char * type() {
	return bionet_hab_get_type((bionet_hab_t *)$self->this);
    }
    const char * id() {
	return bionet_hab_get_id((bionet_hab_t *)$self->this);
    }
    Node * node(const char * node_id) {
	bionet_node_t * n = bionet_hab_get_node_by_id((bionet_hab_t *)$self->this, node_id);
	if (NULL == n) {
	    return NULL;
	}
	bionet_node_increment_ref_count(n);
	return (Node *)bionet_node_get_user_data(n);
    }
    Node * node(const char * node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	bionet_node_t * n = bionet_hab_get_node_by_id_and_uid((bionet_hab_t *)$self->this, node_id, node_uid);
	if (NULL == n) {
	    return NULL;
	}
	bionet_node_increment_ref_count(n);
	return (Node *)bionet_node_get_user_data(n);
    }
    int numNodes() {
	return bionet_hab_get_num_nodes((bionet_hab_t *)$self->this);
    }
    Node * node(unsigned int index) {
	bionet_node_t * n = bionet_hab_get_node_by_index((bionet_hab_t *)$self->this, index);
	if (NULL == n) {
	    return NULL;
	}
	bionet_node_increment_ref_count(n);
	return (Node *)bionet_node_get_user_data(n);
    }
    int add(Node * node) {
	int r = bionet_hab_add_node((bionet_hab_t *)$self->this, node->this);
	if (0 == r) {
	    bionet_node_increment_ref_count(node->this);
	}
	return r;
    }
    Node * remove(const char * node_id) {
	Node * node = (Node *)bionet_node_get_user_data(bionet_hab_remove_node_by_id((bionet_hab_t *)$self->this, node_id));
	if (node) {
	    bionet_node_free(node->this);
	}
	return node;
    }
    Node * remove(const char *node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	Node * node = (Node *)bionet_node_get_user_data(bionet_hab_remove_node_by_id_and_uid((bionet_hab_t *)$self->this, node_id, node_uid));
	if (node) {
	    bionet_node_free(node->this);
	}
	return node;
    }
    int removeAll() {
	return bionet_hab_remove_all_nodes((bionet_hab_t *)$self->this);
    }
    int matches(const char * type, const char * id) {
	return bionet_hab_matches_type_and_id((bionet_hab_t *)$self->this, type, id);
    }
    int isSecure() {
	return bionet_hab_is_secure((bionet_hab_t *)$self->this);
    }
    int numEvents() {
	return bionet_hab_get_num_events((bionet_hab_t *)$self->this);
    }

    Event * eventByIndex(unsigned int index) {
	Event * event;
	bionet_event_t * e = bionet_hab_get_event_by_index((bionet_hab_t *)$self->this, index);
	if (NULL == e) {
	    return NULL;
	}

	event = bionet_event_get_user_data(e);
	if (NULL == event) {
	    event = (Event *)calloc(1, sizeof(Event));
	    if (NULL == event) {
		g_warning("bionet-datapoint.i: Failed to allocate memory for Event wrapper.");
		return NULL;
	    }
	    event->this = e;
	    bionet_event_increment_ref_count(event->this);
	    bionet_event_set_user_data(event->this, event);
	}

	return event;
    } 

    int add(Event * event) {
	bionet_event_increment_ref_count(event->this);
	return bionet_hab_add_event((bionet_hab_t *)$self->this, event->this);
    }
    int add(void (*destructor)(bionet_hab_t * hab, void * user_data),
		      void * user_data) {
	return bionet_hab_add_destructor((bionet_hab_t *)$self->this, destructor, user_data);
    }
}
