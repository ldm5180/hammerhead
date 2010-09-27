%extend Resource {
    Resource(Node * node, bionet_resource_data_type_t datatype, bionet_resource_flavor_t flavor, const char * id) {
	Resource * resource = (Resource *)malloc(sizeof(Resource));
	if (NULL == node) {
	    resource->this = bionet_resource_new(NULL, datatype, flavor, id);
	} else {
	    resource->this = bionet_resource_new(node->this, datatype, flavor, id);
	}
	return resource;
    }

    ~Resource() {
	bionet_resource_free($self->this);
	free($self);
    }

    const char * name() { return bionet_resource_get_name($self->this); }

    const char * local_name() { return bionet_resource_get_local_name($self->this); }

    const char * id() { return bionet_resource_get_id($self->this); }

    bionet_node_t * node() { return bionet_resource_get_node($self->this); }

    bionet_resource_data_type_t datatype() { return bionet_resource_get_data_type($self->this); }

    bionet_resource_flavor_t flavor() { return bionet_resource_get_flavor($self->this); }

    const char * flavorToString(bionet_resource_flavor_t flavor) { return bionet_resource_flavor_to_string(flavor); }

    bionet_resource_flavor_t flavorFromString(const char * flavor_string) { return 
	    bionet_resource_flavor_from_string(flavor_string); 
    }

    const char * datatypeToString(bionet_resource_data_type_t data_type) { 
	return bionet_resource_data_type_to_string(data_type); 
    }

    bionet_resource_data_type_t datatypeFromString(const char * data_type_string) { 
	return bionet_resource_data_type_from_string(data_type_string); 
    }

    int set(const bionet_value_t *content, 
	    const struct timeval *timestamp) {
	return bionet_resource_set($self->this, content, timestamp); 
    }

    int set(int content, const struct timeval * timestamp) {
	char newstr[1024];
	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    return bionet_resource_set_binary($self->this, content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    return bionet_resource_set_uint8($self->this, (uint8_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    return bionet_resource_set_int8($self->this, (int8_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    return bionet_resource_set_uint16($self->this, (uint16_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    return bionet_resource_set_int16($self->this, (int16_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    return bionet_resource_set_uint32($self->this, (uint32_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    return bionet_resource_set_int32($self->this, (int32_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%d", content);
	    return bionet_resource_set_str($self->this, newstr, timestamp); 
	    break;
	default:
	    return -1;
	    break;
	}
    }

    int set(float content, const struct timeval * timestamp) {
	char newstr[1024];
	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_resource_set_float($self->this, content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_resource_set_double($self->this, (double)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%f", content);
	    return bionet_resource_set_str($self->this, newstr, timestamp); 
	    break;
	default: 
	    return -1;
	    break;
	}
    }

    int set(const char * content, const struct timeval * timestamp) {
	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    return bionet_resource_set_binary($self->this, strtol(content, NULL, 0), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    return bionet_resource_set_uint8($self->this, (uint8_t)strtoul(content, NULL, 0), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    return bionet_resource_set_int8($self->this, (int8_t)strtol(content, NULL, 0), timestamp);
	    break;
	    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    return bionet_resource_set_uint16($self->this, (uint16_t)strtoul(content, NULL, 0), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    return bionet_resource_set_int16($self->this, (int16_t)strtol(content, NULL, 0), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    return bionet_resource_set_uint32($self->this, (uint32_t)strtoul(content, NULL, 0), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    return bionet_resource_set_int32($self->this, (int32_t)strtol(content, NULL, 0), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_resource_set_float($self->this, strtof(content, NULL), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_resource_set_double($self->this, strtod(content, NULL), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    return bionet_resource_set_str($self->this, content, timestamp); 
	    break;

	default:
	    return -1;
	    break;
	}
    }

    int set(int content) {
	char newstr[1024];
	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    return bionet_resource_set_binary($self->this, content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    return bionet_resource_set_uint8($self->this, (uint8_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    return bionet_resource_set_int8($self->this, (int8_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    return bionet_resource_set_uint16($self->this, (uint16_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    return bionet_resource_set_int16($self->this, (int16_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    return bionet_resource_set_uint32($self->this, (uint32_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    return bionet_resource_set_int32($self->this, (int32_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_resource_set_float($self->this, (float)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_resource_set_double($self->this, (double)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%d", content);
	    return bionet_resource_set_str($self->this, newstr, NULL); 
	    break;
	default:
	    return -1;
	    break;
	}
    }

    int set(float content) {
	char newstr[1024];
	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_resource_set_float($self->this, content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_resource_set_double($self->this, (double)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%f", content);
	    return bionet_resource_set_str($self->this, newstr, NULL); 
	    break;
	default: 
	    return -1;
	    break;
	}
    }

    int set(const char * content) {
	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    return bionet_resource_set_binary($self->this, strtol(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    return bionet_resource_set_uint8($self->this, (uint8_t)strtoul(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    return bionet_resource_set_int8($self->this, (int8_t)strtol(content, NULL, 0), NULL);
	    break;
	    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    return bionet_resource_set_uint16($self->this, (uint16_t)strtoul(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    return bionet_resource_set_int16($self->this, (int16_t)strtol(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    return bionet_resource_set_uint32($self->this, (uint32_t)strtoul(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    return bionet_resource_set_int32($self->this, (int32_t)strtol(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_resource_set_float($self->this, strtof(content, NULL), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_resource_set_double($self->this, strtod(content, NULL), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    return bionet_resource_set_str($self->this, content, NULL); 
	    break;

	default:
	    return -1;
	    break;
	}
    }

    int numDatapoints() { return bionet_resource_get_num_datapoints($self->this); }
    bionet_datapoint_t * datapoint(unsigned int index) { return bionet_resource_get_datapoint_by_index($self->this, index); }
    void removeDatapoint(unsigned int index) { return bionet_resource_remove_datapoint_by_index($self->this, index); }
    int matchesId(const char * id) { return bionet_resource_matches_id($self->this, id); }
    int matchesHabTypeHabIdNodeIdResourceId(const char *hab_type,
					    const char *hab_id,
					    const char *node_id,
					    const char *resource_id) {
	return bionet_resource_matches_habtype_habid_nodeid_resourceid($self->this,
								       hab_type,
								       hab_id,
								       node_id,
								       resource_id); 
    }
    void setUserData(const void * user_data) { bionet_resource_set_user_data($self->this, user_data); }
    void * userData() { return bionet_resource_get_user_data($self->this); }
    bionet_epsilon_t * epsilon() { return bionet_resource_get_epsilon($self->this); }
    int setEpislon(bionet_epsilon_t * epsilon) { return bionet_resource_set_epsilon($self->this, epsilon); }
    int setDelta(struct timeval delta) { return bionet_resource_set_delta($self->this, delta); }
    const struct timeval * delta() { return bionet_resource_get_delta($self->this); }
    int addDestructor(void (*destructor)(bionet_resource_t * resource, void * user_data),
		      void * user_data) {
	return bionet_resource_add_destructor($self->this, destructor, user_data); 
    }
 }
