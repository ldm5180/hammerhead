%extend Datapoint {
    Datapoint(Resource * resource, Value * value, const struct timeval * timestamp) {
	bionet_datapoint_t * datapoint = bionet_datapoint_new((bionet_resource_t *)resource, (bionet_value_t *)value, timestamp);
	return (Datapoint *)datapoint;
    }

    ~Datapoint() {
	bionet_datapoint_free((bionet_datapoint_t *)$self);
    }
    
    void setValue(Value * value) { bionet_datapoint_set_value((bionet_datapoint_t *)$self, (bionet_value_t *)value); }

    Value * value() { return (Value *)bionet_datapoint_get_value((bionet_datapoint_t *)$self); }

    Resource * resource() { return (Resource *)bionet_datapoint_get_resource((bionet_datapoint_t *)$self); }

    const char * timestampToString() { return bionet_datapoint_timestamp_to_string((bionet_datapoint_t *)$self); }

    void setTimestamp(const struct timeval * timestamp) { bionet_datapoint_set_timestamp((bionet_datapoint_t *)$self, timestamp); }

    struct timeval * timestamp() { return bionet_datapoint_get_timestamp((bionet_datapoint_t *)$self); }

    int isDirty() { return bionet_datapoint_is_dirty((bionet_datapoint_t *)$self); }

    void makeClean() { bionet_datapoint_make_clean((bionet_datapoint_t *)$self); }

    int isEqual(const Datapoint * datapoint) { return bionet_datapoint_iseq((bionet_datapoint_t *)$self, (bionet_datapoint_t *)datapoint); }

    int compareTimeval(const struct timeval * tv) { 
	return bionet_timeval_compare(bionet_datapoint_get_timestamp((bionet_datapoint_t *)$self), tv);
    }

    struct timeval subtractTimeval(const struct timeval * tv) {
	return bionet_timeval_subtract(bionet_datapoint_get_timestamp((bionet_datapoint_t *)$self), tv);
    }

    Datapoint * dup() { return (Datapoint *)bionet_datapoint_dup((bionet_datapoint_t *)$self); }

    int numEvents() { return bionet_datapoint_get_num_events((bionet_datapoint_t *)$self); }

    bionet_event_t * event(unsigned int index) { return bionet_datapoint_get_event_by_index((bionet_datapoint_t *)$self, index); }

    int add(const bionet_event_t * event) { return bionet_datapoint_add_event((bionet_datapoint_t *)$self, event); }

    int add(void (*destructor)(bionet_datapoint_t * datapoint, void * user_data),
		      void * user_data) {
	return bionet_datapoint_add_destructor((bionet_datapoint_t *)$self, destructor, user_data);
    }

}
