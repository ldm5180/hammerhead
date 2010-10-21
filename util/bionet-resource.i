%extend Resource {
    Resource(Node * node, bionet_resource_data_type_t datatype, bionet_resource_flavor_t flavor, const char * id) {
	Resource * resource = (Resource *)malloc(sizeof(Resource));
	if (NULL == resource) {
	    return resource;
	}

	resource->this = bionet_resource_new(node->this, datatype, flavor, id);
	if (NULL == resource->this) {
	    free(resource);
	    return NULL;
	}

	bionet_resource_set_user_data(resource->this, resource);

	return resource;
    }

    ~Resource() {
	int free_me = 0;
	if (0 == bionet_resource_get_ref_count($self->this)) {
	    bionet_resource_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_resource_free($self->this);
	if (free_me) {
	    free($self);
	}
    }

    const char * name() { return bionet_resource_get_name($self->this); }

    const char * local_name() { return bionet_resource_get_local_name($self->this); }

    const char * id() { return bionet_resource_get_id($self->this); }

    Node * node() {
	bionet_node_t * n = bionet_resource_get_node($self->this); 
	if (NULL == n) {
	    return NULL;
	}
	bionet_node_increment_ref_count(n);
	return (Node *)bionet_node_get_user_data(n);
    }

    bionet_resource_data_type_t datatype() { return bionet_resource_get_data_type($self->this); }

    bionet_resource_flavor_t flavor() { return bionet_resource_get_flavor($self->this); }

    const char * flavorToString(bionet_resource_flavor_t flavor) { return bionet_resource_flavor_to_string(flavor); }

    const char * flavorToString() { 
	return bionet_resource_flavor_to_string(bionet_resource_get_flavor($self->this));
    }

    bionet_resource_flavor_t flavorFromString(const char * flavor_string) { 
	return bionet_resource_flavor_from_string(flavor_string); 
    }

    const char * datatypeToString(bionet_resource_data_type_t data_type) { 
	return bionet_resource_data_type_to_string(data_type); 
    }

    const char * datatypeToString() { 
	return bionet_resource_data_type_to_string(bionet_resource_get_data_type($self->this));
    }

    bionet_resource_data_type_t datatypeFromString(const char * data_type_string) { 
	return bionet_resource_data_type_from_string(data_type_string); 
    }

    int set(Value * content, 
	    const struct timeval *timestamp) {

	bionet_datapoint_t * d;
	bionet_value_t * v;
	int r;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    free(datapoint);
	    return 1;
	}

	r = bionet_resource_set($self->this, content->this, timestamp); 

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	} else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int set(int content, const struct timeval * timestamp) {
	char newstr[1024];
	int r;
	bionet_datapoint_t * d;
	bionet_value_t * v;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return 1;
	}

	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    r = bionet_resource_set_binary($self->this, content, timestamp);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    if ((content > UINT8_MAX) || (content < 0)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%d is not uint8_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_uint8($self->this, (uint8_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    if ((content > INT8_MAX) || (content < INT8_MIN)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%d is not int8_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_int8($self->this, (int8_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    if ((content > UINT16_MAX) || (content < 0)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%d is not uint16_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_uint16($self->this, (uint16_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    if ((content > INT16_MAX) || (content < INT16_MIN)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%d is not int16_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_int16($self->this, (int16_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    if ((content > UINT32_MAX) || (content < 0)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%d is not uint32_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_uint32($self->this, (uint32_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    if ((content > INT32_MAX) || (content < INT32_MIN)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%d is not int32_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_int32($self->this, (int32_t)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%d", content);
	    r = bionet_resource_set_str($self->this, newstr, timestamp); 
	    break;
	default:
	    return -1;
	    break;
	}

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	}  else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int set(float content, const struct timeval * timestamp) {
	char newstr[1024];
	int r;
	bionet_datapoint_t * d;
	bionet_value_t * v;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return 1;
	}

	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    r = bionet_resource_set_float($self->this, content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    r = bionet_resource_set_double($self->this, (double)content, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%f", content);
	    r = bionet_resource_set_str($self->this, newstr, timestamp); 
	    break;
	default: 
	    return -1;
	    break;
	}

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	} else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int set(const char * content, const struct timeval * timestamp) {
	int r;
	bionet_datapoint_t * d;
	bionet_value_t * v;
	int icontent;
	unsigned int uicontent;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return 1;
	}

	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    r = bionet_resource_set_binary($self->this, strtol(content, NULL, 0), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    uicontent = strtoul(content, NULL, 0);
	    if ((uicontent > UINT8_MAX) || (uicontent < 0)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s is not uint8_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_uint8($self->this, (uint8_t)uicontent, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    icontent = strtol(content, NULL, 0);
	    if ((icontent > INT8_MAX) || (icontent < INT8_MIN)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s is not int8_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_int8($self->this, (int8_t)icontent, timestamp);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    uicontent = strtoul(content, NULL, 0);
	    if ((uicontent > UINT16_MAX) || (uicontent < 0)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s is not uint16_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_uint16($self->this, (uint16_t)uicontent, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    icontent = strtol(content, NULL, 0);
	    if ((icontent > INT16_MAX) || (icontent < INT8_MIN)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s is not int16_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_int16($self->this, (int16_t)icontent, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    uicontent = strtoul(content, NULL, 0);
	    if ((uicontent > UINT32_MAX) || (uicontent < 0)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s is not uint32_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_uint32($self->this, (uint32_t)uicontent, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    icontent = strtol(content, NULL, 0);
	    if ((icontent > INT32_MAX) || (icontent < INT32_MIN)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s is not int32_t by resource %s is",
		      content, bionet_resource_get_name($self->this));
		return -1;
	    }
	    r = bionet_resource_set_int32($self->this, (int32_t)icontent, timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    r = bionet_resource_set_float($self->this, strtof(content, NULL), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    r = bionet_resource_set_double($self->this, strtod(content, NULL), timestamp); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    r = bionet_resource_set_str($self->this, content, timestamp); 
	    break;

	default:
	    return -1;
	    break;
	}

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	} else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int set(Value * content) {

	bionet_datapoint_t * d;
	bionet_value_t * v;
	int r;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    free(datapoint);
	    return 1;
	}

	r = bionet_resource_set($self->this, content->this, NULL); 

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	} else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int set(int content) {
	char newstr[1024];
	int r;
	bionet_datapoint_t * d;
	bionet_value_t * v;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return 1;
	}

	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    r = bionet_resource_set_binary($self->this, content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    r = bionet_resource_set_uint8($self->this, (uint8_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    r = bionet_resource_set_int8($self->this, (int8_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    r = bionet_resource_set_uint16($self->this, (uint16_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    r = bionet_resource_set_int16($self->this, (int16_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    r = bionet_resource_set_uint32($self->this, (uint32_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    r = bionet_resource_set_int32($self->this, (int32_t)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    r = bionet_resource_set_float($self->this, (float)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    r = bionet_resource_set_double($self->this, (double)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%d", content);
	    r = bionet_resource_set_str($self->this, newstr, NULL); 
	    break;
	default:
	    return -1;
	    break;
	}

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	} else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int set(float content) {
	char newstr[1024];
	int r;
	bionet_datapoint_t * d;
	bionet_value_t * v;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return 1;
	}

	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    r = bionet_resource_set_float($self->this, content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    r = bionet_resource_set_double($self->this, (double)content, NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%f", content);
	    r = bionet_resource_set_str($self->this, newstr, NULL); 
	    break;
	default: 
	    r = -1;
	    break;
	}

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	} else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int set(const char * content) {
	int r;
	bionet_datapoint_t * d;
	bionet_value_t * v;

	Datapoint * datapoint = (Datapoint *)malloc(sizeof(Datapoint));
	if (NULL == datapoint) {
	    return 1;
	}

	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return 1;
	}

	switch (bionet_resource_get_data_type($self->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    r = bionet_resource_set_binary($self->this, strtol(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    r = bionet_resource_set_uint8($self->this, (uint8_t)strtoul(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    r = bionet_resource_set_int8($self->this, (int8_t)strtol(content, NULL, 0), NULL);
	    break;
	    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    r = bionet_resource_set_uint16($self->this, (uint16_t)strtoul(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    r = bionet_resource_set_int16($self->this, (int16_t)strtol(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    r = bionet_resource_set_uint32($self->this, (uint32_t)strtoul(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    r = bionet_resource_set_int32($self->this, (int32_t)strtol(content, NULL, 0), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    r = bionet_resource_set_float($self->this, strtof(content, NULL), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    r = bionet_resource_set_double($self->this, strtod(content, NULL), NULL); 
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    r = bionet_resource_set_str($self->this, content, NULL); 
	    break;

	default:
	    return -1;
	    break;
	}

	if (0 == r) {
	    d = bionet_resource_get_datapoint_by_index($self->this, 0);
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(d, datapoint);
	    bionet_datapoint_increment_ref_count(d);

	    v = bionet_datapoint_get_value(d);
	    value->this = v;
	    if (NULL == value->this) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Resource.set(): No value to set the userdata.");
	    }
	    bionet_value_set_user_data(v, value);
	    bionet_value_increment_ref_count(v);
	} else {
	    free(value);
	    free(datapoint);
	}

	return r;
    }

    int numDatapoints() { return bionet_resource_get_num_datapoints($self->this); }

    Datapoint * datapoint(unsigned int index) { 
	bionet_datapoint_t * d = bionet_resource_get_datapoint_by_index($self->this, index); 
	if (NULL == d) {
	    return NULL;
	}
	bionet_datapoint_increment_ref_count(d);
	return (Datapoint *)bionet_datapoint_get_user_data(d);
    }

    void removeDatapoint(unsigned int index) { 
	return bionet_resource_remove_datapoint_by_index($self->this, index); 
    }

    int matches(const char * id) { return bionet_resource_matches_id($self->this, id); }

    int matches(const char *hab_type,
		const char *hab_id,
		const char *node_id,
		const char *resource_id) {
	return bionet_resource_matches_habtype_habid_nodeid_resourceid($self->this,
								       hab_type,
								       hab_id,
								       node_id,
								       resource_id); 
    }

    bionet_epsilon_t * epsilon() { return bionet_resource_get_epsilon($self->this); }

    int setEpislon(bionet_epsilon_t * epsilon) { return bionet_resource_set_epsilon($self->this, epsilon); }

    int setDelta(struct timeval delta) { return bionet_resource_set_delta($self->this, delta); }

    const struct timeval * delta() { return bionet_resource_get_delta($self->this); }

    int add(void (*destructor)(bionet_resource_t * resource, void * user_data),
		      void * user_data) {
	return bionet_resource_add_destructor($self->this, destructor, user_data); 
    }

    char * __str__() {
	char * resstr = malloc(1024);
	snprintf(resstr, 1024, "%s: %s %s", 
		 bionet_resource_get_name($self->this),
		 bionet_resource_data_type_to_string(bionet_resource_get_data_type($self->this)),
		 bionet_resource_flavor_to_string(bionet_resource_get_flavor($self->this)));
	return resstr;
    }
 }
