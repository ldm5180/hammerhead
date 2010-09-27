%extend Node {
    Node(Hab * hab, const char * id) {
	Node * node = (Node *)bionet_node_new((bionet_hab_t *)hab, id);
	return node;
    }
    ~Node() {
	bionet_node_free((bionet_node_t *)$self);
    }
    const char * name() { return bionet_node_get_name((bionet_node_t *)$self); }

    const char * id() {	return bionet_node_get_id((bionet_node_t *)$self); }

    const uint8_t * uid() { return bionet_node_get_uid((bionet_node_t *)$self); }

    Hab * hab() { return (Hab *)bionet_node_get_hab((bionet_node_t *)$self); }

    int add(Resource * resource) { return bionet_node_add_resource((bionet_node_t *)$self, (bionet_resource_t *)resource); }

    int numResources() { return bionet_node_get_num_resources((bionet_node_t *)$self); }

    Resource * resource(unsigned int index) { 
	return (Resource *)bionet_node_get_resource_by_index((bionet_node_t *)$self, index); 
    }

    Resource * resource(const char * id) { 
	return (Resource *)bionet_node_get_resource_by_id((bionet_node_t *)$self, id); 
    }

    int addStream(bionet_stream_t * stream) { return bionet_node_add_stream((bionet_node_t *)$self, stream); }

    int numStreams() { return bionet_node_get_num_streams((bionet_node_t *)$self); }

    bionet_stream_t * stream(unsigned int index) { return bionet_node_get_stream_by_index((bionet_node_t *)$self, index); }

    bionet_stream_t * stream(const char * id) { return bionet_node_get_stream_by_id((bionet_node_t *)$self, id); }

    int matchesId(const char * id) { return bionet_node_matches_id((bionet_node_t *)$self, id); }

    int matchesHabTypeHabIdNodeId(const char * hab_type,
				  const char * hab_id, 
				  const char * node_id) { 
	return bionet_node_matches_habtype_habid_nodeid((bionet_node_t *)$self, hab_type, hab_id, node_id);
    }

    void setUserData(const void * user_data) { bionet_node_set_user_data((bionet_node_t *)$self, user_data); }

    void * userData() { return bionet_node_get_user_data((bionet_node_t *)$self); }

    int numEvents() { return bionet_node_get_num_events((bionet_node_t *)$self); }

    bionet_event_t * event(unsigned int index) { return bionet_node_get_event_by_index((bionet_node_t *)$self, index); }

    int add(const bionet_event_t * event) { return bionet_node_add_event((bionet_node_t *)$self, event); }

    int add(void (*destructor)(bionet_node_t * node, void * user_data),
		      void * user_data) {
	return bionet_node_add_destructor((bionet_node_t *)$self, destructor, user_data);
    }
 }
