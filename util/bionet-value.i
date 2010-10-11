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
	    break;
	}

	if (NULL == value->this) {
	    free(value);
	    value = NULL;
	} else {
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
	    break;
	}

	if (NULL == value->this) {
	    free(value);
	    value = NULL;
	} else {
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
	    break;
	}

	if (NULL == value->this) {
	    free(value);
	    value = NULL;
	} else {
	    bionet_value_set_user_data(value->this, value);
	}
	
	return value;
    }

    ~Value() {
	int free_me = 0;
	if (0 == bionet_value_get_ref_count($self->this)) {
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
	return (Datapoint *)bionet_datapoint_get_user_data(d);
    }

    char * __str__() {
	return bionet_value_to_str($self->this);
    }

    int __int__() {
	bionet_resource_data_type_t type = bionet_resource_get_data_type(bionet_value_get_resource($self->this));

	switch (type) {

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
		      bionet_resource_data_type_to_string(type));
	    return 0;
	    break;	    
	}
    }

    float __float__() {
	bionet_resource_data_type_t type = bionet_resource_get_data_type(bionet_value_get_resource($self->this));

	switch (type) {

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
		      bionet_resource_data_type_to_string(type));
	    return (float)0;
	    break;	    
	}

    }

}
