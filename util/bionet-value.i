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
	bionet_value_free($self->this);
	free($self);
    }

    Datapoint * datapoint() { 
	bionet_datapoint_t * d = bionet_value_get_datapoint($self->this); 
	bionet_datapoint_increment_ref_count(d);
	return (Datapoint *)bionet_datapoint_get_user_data(d);
    }

    char * __str__() {
	return bionet_value_to_str($self->this);
    }

}
