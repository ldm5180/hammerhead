%extend Datapoint {
    Datapoint(Resource * resource, Value * value, const struct timeval * timestamp) {
	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == resource) {
	    datapoint->this = bionet_resource_new(NULL, value->this, timestamp);
	} else {
	    datapoint->this = bionet_resource_new(resource->this, value->this, timestamp);
	}
	return datapoint;
    }

    ~Datapoint() {
	bionet_datapoint_free($self->this);
	free($self);
    }
    
    void setValue(Value * value) { bionet_datapoint_set_value($self->this, value->this); }

    bionet_value_t * value() { return bionet_datapoint_get_value($self->this); }

    bionet_resource_t * resource() { return bionet_datapoint_get_resource($self->this); }

    const char * timestampToString() { return bionet_datapoint_timestamp_to_string($self->this); }

    void setTimestamp(const struct timeval * timestamp) { bionet_datapoint_set_timestamp($self->this, timestamp); }

    struct timeval * timestamp() { return bionet_datapoint_get_timestamp($self->this); }

    int isDirty() { return bionet_datapoint_is_dirty($self->this); }

    void makeClean() { bionet_datapoint_make_clean($self->this); }

    int isEqual(const * Datapoint) { return bionet_datapoint_iseq($self->this, Datapoint->this); }

    int compareTimeval(const struct timeval * tv) { 
	return bionet_timeval_compare(bionet_datapoint_get_timestamp($self->this), tv);
    }

    struct timeval subtractTimeval(const strut timeval * tv) {
	return bionet_timeval_subtract(bionet_datapoint_get_timestamp($self->this), tv);
    }

    bionet_datapoint_t * dup() { return bionet_datapoint_dup($self->this); }

    int numEvents() { return bionet_datapoint_get_num_events($self->this); }

    bionet_event_t * event(unsigned int index) { return bionet_datapoint_get_event_by_index($self->this, index); }

    int addEvent(const bionet_event_t * event) { return bionet_datapoint_add_event($self->this, event); }

    int addDestructor(void (*destructor)(bionet_datapoint_t * datapoint, void * user_data),
		      void * user_data) {
	return bionet_datapoint_add_destructor($self->this, destructor, user_data);
    }

}
