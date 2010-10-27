%extend Event {
    Event(const struct timeval *timestamp, const char * bdm_id, bionet_event_type_t type) {
	Event * event = (Event *)calloc(1, sizeof(Event));
	if (NULL == event) {
	    g_warning("bionet-event.i: Failed to allocate memory for Event.");
	    return NULL;
	}
	
	event->this = bionet_event_new(timestamp, bdm_id, type);
	if (NULL == event->this) {
	    free(event);
	    return NULL;
	}

	bionet_event_set_user_data(event->this, event);

	return event;
    }

    ~Event() {
	int free_me = 0;
	if (0 == bionet_event_get_ref_count($self->this)) {
	    bionet_event_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_event_free($self->this);
	if (free_me) {
	    free($self);
	}
    }

    const struct timeval * timestamp() {
	return bionet_event_get_timestamp($self->this);
    }

    char * timestampToString() {
	return bionet_event_timestamp_to_string($self->this);
    }

    const char * bdmId() {
	return bionet_event_get_bdm_id($self->this);
    }

    bionet_event_type_t type() {
	return bionet_event_get_type($self->this);
    }

    int64_t seq() {
	return bionet_event_get_seq($self->this);
    }

    void setSeq(int64_t seq) {
	bionet_event_set_seq($self->this, seq);
    }
}
