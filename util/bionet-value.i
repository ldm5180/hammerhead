%extend Value {
    Value(Resource * resource, int content) {
	Value * value;
	char newstr[1024];
	switch (bionet_resource_get_data_type((bionet_resource_t *)resource)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    value = (Value *)bionet_value_new_binary((bionet_resource_t *)resource, content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    value = (Value *)bionet_value_new_uint8((bionet_resource_t *)resource, (uint8_t)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    value = (Value *)bionet_value_new_int8((bionet_resource_t *)resource, (int8_t)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    value = (Value *)bionet_value_new_uint16((bionet_resource_t *)resource, (uint16_t)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    value = (Value *)bionet_value_new_int16((bionet_resource_t *)resource, (int16_t)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    value = (Value *)bionet_value_new_uint32((bionet_resource_t *)resource, (uint32_t)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    value = (Value *)bionet_value_new_int32((bionet_resource_t *)resource, (int32_t)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    value = (Value *)bionet_value_new_float((bionet_resource_t *)resource, (float)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    value = (Value *)bionet_value_new_double((bionet_resource_t *)resource, (double)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%d", content);
	    value = (Value *)bionet_value_new_str((bionet_resource_t *)resource, newstr);
	    break;
	default:
	    value = NULL;
	    break;
	}
	return value;
    }

    Value(Resource * resource, float content) {
	Value * value;
	char newstr[1024];
	switch (bionet_resource_get_data_type((bionet_resource_t *)resource)) {
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    value = (Value *)bionet_value_new_float((bionet_resource_t *)resource, (float)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    value = (Value *)bionet_value_new_double((bionet_resource_t *)resource, (double)content);
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    snprintf(newstr, sizeof(newstr), "%f", content);
	    value = (Value *)bionet_value_new_str((bionet_resource_t *)resource, newstr);
	    break;
	default:
	    value = NULL;
	    break;
	}
	return value;
    }

    Value(Resource * resource, char * content) {
	Value * value = (Value *)malloc(sizeof(Value));
	if (NULL == value) {
	    return value;
	}
	switch (bionet_resource_get_data_type((bionet_resource_t *)resource)) {
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    value = (Value *)bionet_value_new_binary((bionet_resource_t *)resource, strtol(content, NULL, 0));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    value = (Value *)bionet_value_new_uint8((bionet_resource_t *)resource, strtoul(content, NULL, 0));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	    value = (Value *)bionet_value_new_int8((bionet_resource_t *)resource, strtol(content, NULL, 0));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    value = (Value *)bionet_value_new_uint16((bionet_resource_t *)resource, strtoul(content, NULL, 0));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	    value = (Value *)bionet_value_new_int16((bionet_resource_t *)resource, strtol(content, NULL, 0));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    value = (Value *)bionet_value_new_uint32((bionet_resource_t *)resource, strtoul(content, NULL, 0));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	    value = (Value *)bionet_value_new_int32((bionet_resource_t *)resource, strtol(content, NULL, 0));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    value = (Value *)bionet_value_new_float((bionet_resource_t *)resource, strtof(content, NULL));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    value = (Value *)bionet_value_new_double((bionet_resource_t *)resource, strtod(content, NULL));
	    break;
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    value = (Value *)bionet_value_new_str((bionet_resource_t *)resource, content);
	    break;
	default:
	    value = NULL;
	    break;
	}
	return value;
    }

    ~Value() {
	bionet_value_free((bionet_value_t *)$self);
    }

    Datapoint * datapoint() { return (Datapoint *)bionet_value_get_datapoint((bionet_value_t *)$self); }



}
