%extend Epsilon {
    
    Epsilon(Resource * resource, int content) {
	Epsilon * epsilon = (Epsilon *)calloc(1, sizeof(Epsilon));
	if (NULL == epsilon) {
	    g_warning("bionet-epsilon.i: Failed to allocate memory for epsilon");
	    return NULL;
	}

	bionet_epsilon_t * e;
	switch (bionet_resource_get_data_type(resource->this)) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    e = bionet_epsilon_new_binary((int)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    e = bionet_epsilon_new_uint8((uint8_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    e = bionet_epsilon_new_int8((int8_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    e = bionet_epsilon_new_uint16((uint16_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    e = bionet_epsilon_new_int16((int16_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    e = bionet_epsilon_new_uint32((uint32_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    e = bionet_epsilon_new_int32((int32_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    e = bionet_epsilon_new_float((float)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    e = bionet_epsilon_new_double((double)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    e = bionet_epsilon_new_str((int)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-epsilon.i: Invalid resource type. Cannot create Epsilon.");
	    free(epsilon);
	    return NULL;
	    break;
	}

	if (NULL == e) {
	    g_warning("bionet-epsilon.i: Failed to get a new epsilon.");
	    free(epsilon);
	    return NULL;
	}

	epsilon->this = e;
	epsilon->datatype = bionet_resource_get_data_type(resource->this);
	bionet_epsilon_set_user_data(epsilon->this, epsilon);

	return epsilon;
    }

    Epsilon(Resource * resource, float content) {
	Epsilon * epsilon = (Epsilon *)calloc(1, sizeof(Epsilon));
	if (NULL == epsilon) {
	    g_warning("bionet-epsilon.i: Failed to allocate memory for epsilon");
	    return NULL;
	}

	bionet_epsilon_t * e;
	switch (bionet_resource_get_data_type(resource->this)) {

	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    e = bionet_epsilon_new_float((float)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    e = bionet_epsilon_new_double((double)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-epsilon.i: Invalid resource type. Cannot create Epsilon.");
	    free(epsilon);
	    return NULL;
	    break;
	}

	if (NULL == e) {
	    g_warning("bionet-epsilon.i: Failed to get a new epsilon.");
	    free(epsilon);
	    return NULL;
	}

	epsilon->this = e;
	epsilon->datatype = bionet_resource_get_data_type(resource->this);
	bionet_epsilon_set_user_data(epsilon->this, epsilon);

	return epsilon;
    }

    Epsilon(Resource * resource, char * content) {
	Epsilon * epsilon = (Epsilon *)calloc(1, sizeof(Epsilon));
	if (NULL == epsilon) {
	    g_warning("bionet-epsilon.i: Failed to allocate memory for epsilon");
	    return NULL;
	}

	bionet_epsilon_t * e;
	switch (bionet_resource_get_data_type(resource->this)) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    e = bionet_epsilon_new_binary((int)strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    e = bionet_epsilon_new_uint8((uint8_t)strtoul(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    e = bionet_epsilon_new_int8((int8_t)strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    e = bionet_epsilon_new_uint16((uint16_t)strtoul(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    e = bionet_epsilon_new_int16((int16_t)strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    e = bionet_epsilon_new_uint32((uint32_t)strtoul(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    e = bionet_epsilon_new_int32((int32_t)strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    e = bionet_epsilon_new_float((float)strtof(content, NULL));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    e = bionet_epsilon_new_double((double)strtod(content, NULL));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    e = bionet_epsilon_new_str((int)strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-epsilon.i: Invalid resource type. Cannot create Epsilon.");
	    free(epsilon);
	    return NULL;
	    break;
	}

	if (NULL == e) {
	    g_warning("bionet-epsilon.i: Failed to get a new epsilon.");
	    free(epsilon);
	    return NULL;
	}

	epsilon->this = e;
	epsilon->datatype = bionet_resource_get_data_type(resource->this);
	bionet_epsilon_set_user_data(epsilon->this, epsilon);

	return epsilon;
    }

    ~Epsilon() {
	int free_me = 0;
	if (0 == bionet_epsilon_get_ref_count($self->this)) {
	    bionet_epsilon_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_epsilon_free($self->this);
	if (free_me) {
	    free($self);
	}
    }

    char * __str__() {
	return bionet_epsilon_to_str($self->this, $self->data_type);
    }

    float value() {
	int r = -1;
	float value;
	switch ($self->datatype) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int content;
	    r = bionet_epsilon_get_binary($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t content;
	    r = bionet_epsilon_get_uint8($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t content;
	    r = bionet_epsilon_get_int8($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t content;
	    r = bionet_epsilon_get_uint16($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t content;
	    r = bionet_epsilon_get_int16($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t content;
	    r = bionet_epsilon_get_uint32($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t content;
	    r = bionet_epsilon_get_int32($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float content;
	    r = bionet_epsilon_get_float($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double content;
	    r = bionet_epsilon_get_double($self->this, &content);
	    value = (float)content;
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	{
	    int content;
	    r = bionet_epsilon_get_str($self->this, &content);
	    value = (float)content;
	    break;
	}

	default:
	    g_warning("Invalid data type for epsilon");
	    break;
	}

	if (r) {
	    g_warning("bionet-epsilon.i: Failed to get the value of the epsilon.");
	}
	
	return value;
    }
}
