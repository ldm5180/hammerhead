%extend Stream {

    Stream(Node * node, const char * id, bionet_stream_direction_t direction, const char * type) {
	Stream * stream = (Stream *)calloc(1, sizeof(Stream));
	if (NULL == stream) {
	    g_warning("bionet-stream.i: Failed to allocate stream.");
	    return NULL;
	}

	stream->this = bionet_stream_new(node->this, id, direction, type);
	if (NULL == stream->this) {
	    return NULL;
	}

	bionet_stream_set_user_data(stream->this, stream);

	return stream;
    }

    ~Stream() {
	int free_me = 0;
	if (0 == bionet_stream_get_ref_count($self->this)) {
	    bionet_stream_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_resource_free($self->this);
	if (free_me) {
	    free($self);
	}
    }

    const char * id() {
	return bionet_stream_get_id($self->this);
    }

    const char * type() {
	return bionet_stream_get_type($self->this);
    }

    Node * node() {
	Node * node;
	bionet_node_t * n = bionet_stream_get_node($self->this);
	if (NULL == n) {
	    g_warning("bionet-stream.i: Failed to get node from stream.");
	    return NULL;
	}
	
	node = (Node *)bionet_node_get_user_data(n);
	if (NULL == node) {
	    node = (Node *)calloc(1, sizeof(Node));
	    if (NULL == node) {
		g_warning("bionet-stream.i: Failed to allocate memory to wrap node.");
		return NULL;
	    }
	    node->this = n;
	    bionet_node_set_user_data(node->this, node);
	}

	bionet_node_increment_ref_count(node->this);
	return node;
    }

    bionet_stream_direction_t direction() {
	return bionet_stream_get_direction($self->this);
    }

    bionet_stream_direction_t direction(const char * direction) {
	return bionet_stream_get_direction_from_string(direction);
    }

    const char * directionToString() {
	return bionet_stream_direction_to_string(bionet_stream_get_direction($self->this));
    }

    const char * name() {
	return bionet_stream_get_name($self->this);
    }

    const char * localName() {
	return bionet_stream_get_local_name($self->this);
    }

    
}
