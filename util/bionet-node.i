%extend Node {
    Node(Hab * hab, const char * id) {
	Node * node = (Node *)malloc(sizeof(Node));
	if (NULL == node) {
	    return node;
	}

	node->this = bionet_node_new((bionet_hab_t *)hab->this, id);
	if (NULL == node->this) {
	    free(node);
	    return NULL;
	}

	bionet_node_set_user_data(node->this, node);
	
	return node;
    }

    ~Node() {
	int free_me = 0;
	if (0 == bionet_node_get_ref_count($self->this)) {
	    bionet_node_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_node_free($self->this);
	if (free_me) {
	    free($self);
	}
    }

    const char * name() { return bionet_node_get_name((bionet_node_t *)$self->this); }

    const char * id() {	return bionet_node_get_id((bionet_node_t *)$self->this); }

    const uint8_t * uid() { return bionet_node_get_uid((bionet_node_t *)$self->this); }

    Hab * hab() {
	bionet_hab_t * h = bionet_node_get_hab($self->this);
	bionet_hab_increment_ref_count(h);
	return (Hab *)bionet_hab_get_user_data(h); 
    }

    int add(Resource * resource) { 
	int r = bionet_node_add_resource($self->this, resource->this); 
	if (0 == r) {
	    bionet_resource_increment_ref_count(resource->this);
	}
	return r;
    }

    int numResources() { return bionet_node_get_num_resources((bionet_node_t *)$self->this); }

    Resource * resource(unsigned int index) { 
	bionet_resource_t * r = bionet_node_get_resource_by_index($self->this, index);
	if (NULL == r) {
	    return NULL;
	}
	bionet_resource_increment_ref_count(r);
	return (Resource *)bionet_resource_get_user_data(r);
    }

    Resource * resource(const char * id) { 
	bionet_resource_t * r = bionet_node_get_resource_by_id($self->this, id);
	if (NULL == r) {
	    return NULL;
	}
	bionet_resource_increment_ref_count(r);
	return (Resource *)bionet_resource_get_user_data(r);
    }

    int add(Stream * stream) { 
	bionet_stream_increment_ref_count(stream->this);
	return bionet_node_add_stream($self->this, stream->this); 
    }

    int numStreams() { return bionet_node_get_num_streams((bionet_node_t *)$self->this); }

    Stream * stream(unsigned int index) { 
	Stream * stream;
	bionet_stream_t * s = bionet_node_get_stream_by_index($self->this, index); 
	if (NULL == s) {
	    return NULL;
	}
	
	stream = bionet_stream_get_user_data(s);
	if (NULL == stream) {
	    stream = (Stream *)calloc(1, sizeof(Stream));
	    if (NULL == stream) {
		g_warning("bionet-node.i: Failed to allocate memory for stream.");
		return NULL;
	    }

	    stream->this = s;
	    bionet_stream_set_user_data(stream->this, stream);
	}
	
	bionet_stream_increment_ref_count(stream->this);
	return stream;
    }

    Stream * stream(const char * id) { 
	Stream * stream;
	bionet_stream_t * s = bionet_node_get_stream_by_id((bionet_node_t *)$self->this, id); 
	if (NULL == s) {
	    return NULL;
	}
	
	stream = bionet_stream_get_user_data(s);
	if (NULL == stream) {
	    stream = (Stream *)calloc(1, sizeof(Stream));
	    if (NULL == stream) {
		g_warning("bionet-node.i: Failed to allocate memory for stream.");
		return NULL;
	    }

	    stream->this = s;
	    bionet_stream_set_user_data(stream->this, stream);
	}
	
	bionet_stream_increment_ref_count(stream->this);
	return stream;
    }

    int matches(const char * id) { return bionet_node_matches_id((bionet_node_t *)$self->this, id); }

    int matches(const char * hab_type,
		const char * hab_id, 
		const char * node_id) { 
	return bionet_node_matches_habtype_habid_nodeid((bionet_node_t *)$self->this, hab_type, hab_id, node_id);
    }

    int numEvents() { return bionet_node_get_num_events((bionet_node_t *)$self->this); }

    Event * event(unsigned int index) { 
	Event * event;
	bionet_event_t * e = bionet_node_get_event_by_index((bionet_node_t *)$self->this, index); 
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
	return bionet_node_add_event((bionet_node_t *)$self->this, event->this); 
    }

    int add(void (*destructor)(bionet_node_t * node, void * user_data),
		      void * user_data) {
	return bionet_node_add_destructor((bionet_node_t *)$self->this, destructor, user_data);
    }
 }
