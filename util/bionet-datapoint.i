%extend Datapoint {
    Datapoint(Resource * resource, Value * value, const struct timeval * timestamp) {
	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return NULL;
	}

	datapoint->this = bionet_datapoint_new(resource->this, value->this, timestamp);
	if (NULL == datapoint->this) {
	    free(datapoint);
	    return NULL;
	}

	bionet_datapoint_set_user_data(datapoint->this, datapoint);

	return datapoint;
    }

    ~Datapoint() {
	int free_me = 0;
	if (0 == bionet_datapoint_get_ref_count($self->this)) {
	    bionet_datapoint_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_datapoint_free($self->this);
	if (free_me) {
	    free($self);
	}
    }
    
    void setValue(Value * value) { 
	bionet_value_increment_ref_count(value->this);
	bionet_datapoint_set_value($self->this, value->this); 
    }

    Value * value() {
	Value * value;
	bionet_value_t * v = bionet_datapoint_get_value($self->this); 

	bionet_value_increment_ref_count(v);

	value = (Value *)bionet_value_get_user_data(v);
	if (NULL == value) {
	    value = (Value *)calloc(1, sizeof(Value));
	    if (NULL == value) {
		g_warning("Failed to allocate memory to wrap bionet_value_t");
		return NULL;
	    }
	    value->this = v;
	    bionet_value_set_user_data(value->this, value);
	}

	return value;
    }

    Resource * resource() {
	bionet_resource_t * r = bionet_datapoint_get_resource($self->this); 

	bionet_resource_increment_ref_count(r);

	Resource * resource = (Resource *)bionet_resource_get_user_data(r);
	if (NULL == resource) {
	    resource = (Resource *)calloc(1, sizeof(Resource));
	    if (NULL == resource) {
		g_warning("Failed to allocate memory to wrap bionet_resource_t");
		return NULL;
	    }
	    resource->this = r;
	    bionet_resource_set_user_data(resource->this, resource);
	}

	return resource;
    }

    const char * timestampToString() { return bionet_datapoint_timestamp_to_string($self->this); }

    void setTimestamp(const struct timeval * timestamp) { bionet_datapoint_set_timestamp($self->this, timestamp); }

    struct timeval * timestamp() { return bionet_datapoint_get_timestamp($self->this); }

    int isDirty() { return bionet_datapoint_is_dirty($self->this); }

    void makeClean() { bionet_datapoint_make_clean($self->this); }

    int isEqual(const Datapoint * datapoint) { 
	return bionet_datapoint_iseq($self->this, datapoint->this); 
    }

    int compareTimeval(const struct timeval * tv) { 
	return bionet_timeval_compare(bionet_datapoint_get_timestamp($self->this), tv);
    }

    struct timeval subtractTimeval(const struct timeval * tv) {
	return bionet_timeval_subtract(bionet_datapoint_get_timestamp($self->this), tv);
    }

    Datapoint * dup() { return (Datapoint *)bionet_datapoint_dup($self->this); }

    int numEvents() { return bionet_datapoint_get_num_events($self->this); }

    Event * event(unsigned int index) { 
	Event * event;
	bionet_event_t * e = bionet_datapoint_get_event_by_index($self->this, index); 
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
	return bionet_datapoint_add_event($self->this, event->this); 
    }

    int add(void (*destructor)(bionet_datapoint_t * datapoint, void * user_data),
		      void * user_data) {
	return bionet_datapoint_add_destructor($self->this, destructor, user_data);
    }

    char * __str__() {
	char * newstr = (char *)malloc(2048);
	if (NULL == newstr) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Datapoint.__str__(): Failed to allocate memory.");
	    return NULL;
	}
	bionet_value_t * value = bionet_datapoint_get_value($self->this);
	if (NULL == value) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Datapoint.__str__(): Value is NULL.");
	    return NULL;
	}
	int r = snprintf(newstr, 2048, "%s @ %s", 
			 bionet_value_as_str(value),
			 bionet_datapoint_timestamp_to_string($self->this));
	if (r >= 2048) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Datapoint.__str__(): String to too long.");
	    return NULL;
	}
	return newstr;
    }
}
