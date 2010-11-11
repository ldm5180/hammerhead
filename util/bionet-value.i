%extend Value {
    Value(Resource * resource, int content) {
	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	char newstr[1024];

	switch (bionet_resource_get_data_type(resource->this)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    value->this = bionet_value_new_binary(resource->this, content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    value->this = bionet_value_new_uint8(resource->this, (uint8_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    value->this = bionet_value_new_int8(resource->this, (int8_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    value->this = bionet_value_new_uint16(resource->this, (uint16_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    value->this = bionet_value_new_int16(resource->this, (int16_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    value->this = bionet_value_new_uint32(resource->this, (uint32_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    value->this = bionet_value_new_int32(resource->this, (int32_t)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    value->this = bionet_value_new_float(resource->this, (float)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    value->this = bionet_value_new_double(resource->this, (double)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%d", content);
	    value->this = bionet_value_new_str(resource->this, newstr);
	    break;

	default:
	    free(value);
	    value = NULL;
	    g_warning("bionet-value.i: Failed to create value of unknown type.");
	    break;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value(Resource * resource, float content) {
	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	char newstr[1024];

	switch (bionet_resource_get_data_type(resource->this)) {
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    value->this = bionet_value_new_float(resource->this, (float)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    value->this = bionet_value_new_double(resource->this, (double)content);
	    break;

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%f", content);
	    value->this = bionet_value_new_str(resource->this, newstr);
	    break;

	default:
	    value = NULL;
	    g_warning("bionet-value.i: Failed to create value of unknown type.");
	    break;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}

	return value;
    }

    Value(Resource * resource, char * content) {
	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return value;
	}

	switch (bionet_resource_get_data_type(resource->this)) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    value->this = bionet_value_new_binary(resource->this, strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    value->this = bionet_value_new_uint8(resource->this, strtoul(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    value->this = bionet_value_new_int8(resource->this, strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    value->this = bionet_value_new_uint16(resource->this, strtoul(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    value->this = bionet_value_new_int16(resource->this, strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    value->this = bionet_value_new_uint32(resource->this, strtoul(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    value->this = bionet_value_new_int32(resource->this, strtol(content, NULL, 0));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    value->this = bionet_value_new_float(resource->this, strtof(content, NULL));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    value->this = bionet_value_new_double(resource->this, strtod(content, NULL));
	    break;

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    value->this = bionet_value_new_str(resource->this, content);
	    break;

	default:
	    value = NULL;
	    g_warning("bionet-value.i: Failed to create value of unknown type.");
	    break;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	
	return value;
    }

    ~Value() {
	int free_me = 0;
	if (0 == bionet_value_get_ref_count($self->this)) {
	    bionet_value_set_user_data($self->this, NULL);
	    free_me = 1;
	}
	bionet_value_free($self->this);
	if (free_me) {
	    free($self);
	}
    }

    Datapoint * datapoint() { 
	bionet_datapoint_t * d = bionet_value_get_datapoint($self->this); 
	
	bionet_datapoint_increment_ref_count(d);

	Datapoint * datapoint = (Datapoint *)bionet_datapoint_get_user_data(d);
	if (NULL == datapoint) {
	    datapoint = (Datapoint *)calloc(1, sizeof(Datapoint));
	    if (NULL == datapoint) {
		g_warning("Failed to allocate memory to wrap bionet_datapoint_t");
		return NULL;
	    }
	    datapoint->this = d;
	    bionet_datapoint_set_user_data(datapoint->this, datapoint);
	}

	return datapoint;
    }

    int set(int new) {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    return bionet_value_set_binary($self->this, new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    return bionet_value_set_uint8($self->this, (uint8_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    return bionet_value_set_int8($self->this, (int8_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    return bionet_value_set_uint16($self->this, (uint16_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    return bionet_value_set_int16($self->this, (int16_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    return bionet_value_set_uint32($self->this, (uint32_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    return bionet_value_set_int32($self->this, (int32_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_value_set_float($self->this, (float)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_value_set_double($self->this, (double)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	{
	    char buf[1024];
	    int r;
	    r = snprintf(buf, sizeof(buf), "%d", new); 
	    if (r >= sizeof(buf)) {
		g_warning("bionet-value.i: Failed to convert integer to string.");
		return -1;
	    }
	    return bionet_value_set_str($self->this, buf);
	    break;
	}
	default:
	    g_warning("bionet-value.i: Unknown data type.");
	    return -1;
	}
    }

    int set(float new) {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    return bionet_value_set_binary($self->this, new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    return bionet_value_set_uint8($self->this, (uint8_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    return bionet_value_set_int8($self->this, (int8_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    return bionet_value_set_uint16($self->this, (uint16_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    return bionet_value_set_int16($self->this, (int16_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    return bionet_value_set_uint32($self->this, (uint32_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    return bionet_value_set_int32($self->this, (int32_t)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_value_set_float($self->this, (float)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_value_set_double($self->this, (double)new);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	{
	    char buf[1024];
	    int r;
	    r = snprintf(buf, sizeof(buf), "%f", new); 
	    if (r >= sizeof(buf)) {
		g_warning("bionet-value.i: Failed to convert integer to string.");
		return -1;
	    }
	    return bionet_value_set_str($self->this, buf);
	    break;
	}
	default:
	    g_warning("bionet-value.i: Unknown data type.");
	    return -1;
	}
    }

    int set(char * new) {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    return bionet_value_set_binary($self->this, strtol(new, NULL, 0));
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    return bionet_value_set_uint8($self->this, (uint8_t)strtoul(new, NULL, 0));
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    return bionet_value_set_int8($self->this, (int8_t)strtol(new, NULL, 0));
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    return bionet_value_set_uint16($self->this, (uint16_t)strtoul(new, NULL, 0));
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    return bionet_value_set_int16($self->this, (int16_t)strtol(new, NULL, 0));
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    return bionet_value_set_uint32($self->this, (uint32_t)strtoul(new, NULL, 0));
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    return bionet_value_set_int32($self->this, (int32_t)strtol(new, NULL, 0));
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    return bionet_value_set_float($self->this, strtof(new, NULL));
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    return bionet_value_set_double($self->this, strtod(new, NULL));
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    return bionet_value_set_str($self->this, new);
	default:
	    g_warning("bionet-value.i: Unknown data type.");
	    return -1;
	}
    }

    char * __str__() {
	return bionet_value_to_str($self->this);
    }

    int __int__() {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    bionet_value_get_binary($self->this, &val);
	    return val;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    return (int)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    return (int)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    return (int)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    return (int)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    return (int)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    return (int)val;
	}

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    return 0;
	    break;	    
	}
    }

    float __float__() {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    return (float)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    return (float)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    return (float)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    return (float)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    return (float)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    return (float)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    return (float)val;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    return (float)val;
	}

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    return (float)0;
	    break;	    
	}

    }


}
