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
	bionet_datapoint_free($self->this);
	free($self);
    }
    
    void setValue(Value * value) { 
	bionet_value_increment_ref_count(value->this);
	bionet_datapoint_set_value($self->this, value->this); 
    }

    Value * value() {
	bionet_value_t * v = bionet_datapoint_get_value($self->this); 
	bionet_value_increment_ref_count(v);
	return (Value *)bionet_value_get_user_data(v);
    }

    Resource * resource() {
	bionet_resource_t * r = bionet_datapoint_get_resource($self->this); 
	bionet_resource_increment_ref_count(r);
	return (Resource *)bionet_resource_get_user_data(r);
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

    bionet_event_t * event(unsigned int index) { return bionet_datapoint_get_event_by_index($self->this, index); }

    int add(const bionet_event_t * event) { return bionet_datapoint_add_event($self->this, event); }

    int add(void (*destructor)(bionet_datapoint_t * datapoint, void * user_data),
		      void * user_data) {
	return bionet_datapoint_add_destructor($self->this, destructor, user_data);
    }

    char * __str__() {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Datapoint.__str__(): Called.");
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
			 bionet_value_to_str(value),
			 bionet_datapoint_timestamp_to_string($self->this));
	if (r >= 2048) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Datapoint.__str__(): String to too long.");
	    return NULL;
	}
	return newstr;
    }
}
