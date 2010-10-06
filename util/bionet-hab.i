%inline %{

%}

%extend Hab {
    Hab(const char * type, const char * id) {
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	if (NULL == hab) {
	    return hab;
	}
	
	hab->this = bionet_hab_new(type, id);
	if (NULL == hab->this) {
	    free(hab);
	    return NULL;
	}

	bionet_hab_set_user_data(hab->this, hab);

	hab->fd = -1;
	hab->set_resource_callback = NULL;

	return hab;
    }
    Hab() {
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	if (NULL == hab) {
	    return hab;
	}
	
	hab->this = bionet_hab_new(NULL, NULL);
	if (NULL == hab->this) {
	    free(hab);
	    return NULL;
	}

	bionet_hab_set_user_data(hab->this, hab);

	hab->fd = -1;
	hab->set_resource_callback = NULL;

	return hab;
    }
    ~Hab() {
	int free_me = 0;
	if (0 == bionet_hab_get_ref_count($self->this)) {
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
	bionet_node_increment_ref_count(n);
	return (Node *)bionet_node_get_user_data(n);
    }
    Node * node(const char * node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	bionet_node_t * n = bionet_hab_get_node_by_id_and_uid((bionet_hab_t *)$self->this, node_id, node_uid);
	bionet_node_increment_ref_count(n);
	return (Node *)bionet_node_get_user_data(n);
    }
    int numNodes() {
	return bionet_hab_get_num_nodes((bionet_hab_t *)$self->this);
    }
    Node * node(unsigned int index) {
	bionet_node_t * n = bionet_hab_get_node_by_index((bionet_hab_t *)$self->this, index);
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
	return (Node *)bionet_node_get_user_data(bionet_hab_remove_node_by_id((bionet_hab_t *)$self->this, node_id));
    }
    Node * remove(const char *node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	return (Node *)bionet_node_get_user_data(bionet_hab_remove_node_by_id_and_uid((bionet_hab_t *)$self->this, node_id, node_uid));
    }
    int removeAll() {
	return bionet_hab_remove_all_nodes((bionet_hab_t *)$self->this);
    }
    int matchesTypeAndId(const char * type, const char * id) {
	return bionet_hab_matches_type_and_id((bionet_hab_t *)$self->this, type, id);
    }
    int isSecure() {
	return bionet_hab_is_secure((bionet_hab_t *)$self->this);
    }
    int numEvents() {
	return bionet_hab_get_num_events((bionet_hab_t *)$self->this);
    }
    bionet_event_t * eventByIndex(unsigned int index) {
	return bionet_hab_get_event_by_index((bionet_hab_t *)$self->this, index);
    } 
    int add(bionet_event_t * event) {
	return bionet_hab_add_event((bionet_hab_t *)$self->this, event);
    }
    int add(void (*destructor)(bionet_hab_t * hab, void * user_data),
		      void * user_data) {
	return bionet_hab_add_destructor((bionet_hab_t *)$self->this, destructor, user_data);
    }

}
