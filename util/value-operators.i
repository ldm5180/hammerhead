%extend Value {
    float __abs__() {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    bionet_value_get_binary($self->this, &val);
	    return (float)val;
	}
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
	    return (float)abs(val);
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
	    return (float)abs(val);
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
	    return (float)abs(val);
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    return (float)fabsf(val);
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    return (float)fabs(val);
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

    Value * __add__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val += (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val += (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val += (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val += (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val += (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val += (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val += (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val += (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __add__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val += (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val += (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val += (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val += (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val += (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val += (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val += (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val += (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __add__(Value * other) {
	if ((bionet_resource_get_data_type(bionet_value_get_resource($self->this))) != (bionet_resource_get_data_type(bionet_value_get_resource(other->this)))) {
	    g_warning("bionet-value.i: Cannot add Values of different types.");
	    return NULL;
	}

	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t rval = 0;
	    bionet_value_get_uint8(other->this, &rval);
	    bionet_value_get_uint8($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t rval = 0;
	    bionet_value_get_int8(other->this, &rval);
	    bionet_value_get_int8($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t rval = 0;
	    bionet_value_get_uint16(other->this, &rval);
	    bionet_value_get_uint16($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t rval = 0;
	    bionet_value_get_int16(other->this, &rval);
	    bionet_value_get_int16($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t rval = 0;
	    bionet_value_get_uint32(other->this, &rval);
	    bionet_value_get_uint32($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t rval = 0;
	    bionet_value_get_int32(other->this, &rval);
	    bionet_value_get_int32($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float rval = 0;
	    bionet_value_get_float(other->this, &rval);
	    bionet_value_get_float($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double rval = 0;
	    bionet_value_get_double(other->this, &rval);
	    bionet_value_get_double($self->this, &val);
	    val += rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __radd__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val += (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val += (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val += (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val += (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val += (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val += (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val += (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val += (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __radd__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val += (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val += (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val += (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val += (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val += (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val += (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val += (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val += (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    int __cmp__(int rval) {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val;
	    bionet_value_get_binary($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val;
	    bionet_value_get_uint8($self->this, &val);
	    if (val > (uint8_t)rval) {
		return 1;
	    } else if (val < (uint8_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val;
	    bionet_value_get_int8($self->this, &val);
	    if (val > (int8_t)rval) {
		return 1;
	    } else if (val < (int8_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val;
	    bionet_value_get_uint16($self->this, &val);
	    if (val > (uint16_t)rval) {
		return 1;
	    } else if (val < (uint16_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val;
	    bionet_value_get_int16($self->this, &val);
	    if (val > (int16_t)rval) {
		return 1;
	    } else if (val < (int16_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val;
	    bionet_value_get_uint32($self->this, &val);
	    if (val > (uint32_t)rval) {
		return 1;
	    } else if (val < (uint32_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val;
	    bionet_value_get_int32($self->this, &val);
	    if (val > (int32_t)rval) {
		return 1;
	    } else if (val < (int32_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val;
	    bionet_value_get_float($self->this, &val);
	    if (val > (float)rval) {
		return 1;
	    } else if (val < (float)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val;
	    bionet_value_get_double($self->this, &val);
	    if (val > (double)rval) {
		return 1;
	    } else if (val < (double)rval) {
		return -1;
	    }
	    return 0;
	} 
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	{
	    char * val;
	    char other[1024];
	    int r;
	    r = snprintf(other, sizeof(other), "%d", rval);
	    if (r >= sizeof(other)) {
		g_warning("bionet-value.i: Failed to convert int to string for comparison.");
		return 1;
	    }
	    bionet_value_get_str($self->this, &val);
	    return strncmp(val, other, sizeof(other));
	}
	default:
	    g_warning("bionet-value.i: Unknown data type in comparison.");
	    return 1;
	}
    }

    int __cmp__(float rval) {
	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val;
	    bionet_value_get_binary($self->this, &val);
	    if (val > (int)rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val;
	    bionet_value_get_uint8($self->this, &val);
	    if (val > (uint8_t)rval) {
		return 1;
	    } else if (val < (uint8_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val;
	    bionet_value_get_int8($self->this, &val);
	    if (val > (int8_t)rval) {
		return 1;
	    } else if (val < (int8_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val;
	    bionet_value_get_uint16($self->this, &val);
	    if (val > (uint16_t)rval) {
		return 1;
	    } else if (val < (uint16_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val;
	    bionet_value_get_int16($self->this, &val);
	    if (val > (int16_t)rval) {
		return 1;
	    } else if (val < (int16_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val;
	    bionet_value_get_uint32($self->this, &val);
	    if (val > (uint32_t)rval) {
		return 1;
	    } else if (val < (uint32_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val;
	    bionet_value_get_int32($self->this, &val);
	    if (val > (int32_t)rval) {
		return 1;
	    } else if (val < (int32_t)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val;
	    bionet_value_get_float($self->this, &val);
	    if (val > (float)rval) {
		return 1;
	    } else if (val < (float)rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val;
	    bionet_value_get_double($self->this, &val);
	    if (val > (double)rval) {
		return 1;
	    } else if (val < (double)rval) {
		return -1;
	    }
	    return 0;
	} 
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	{
	    char * val;
	    char other[1024];
	    int r;
	    r = snprintf(other, sizeof(other), "%f", rval);
	    if (r >= sizeof(other)) {
		g_warning("bionet-value.i: Failed to convert int to string for comparison.");
		return 1;
	    }
	    bionet_value_get_str($self->this, &val);
	    return strncmp(val, other, sizeof(other));
	}
	default:
	    g_warning("bionet-value.i: Unknown data type in comparison.");
	    return 1;
	}
    }

    int __cmp__(Value * other) {
	if (NULL == other) {
	    g_warning("bionet-value.i: NULL passed into comparator.");
	    return 1;
	}
	
	if ((bionet_resource_get_data_type(bionet_value_get_resource($self->this))) != (bionet_resource_get_data_type(bionet_value_get_resource(other->this)))) {
	    g_warning("bionet-value.i: Cannot add Values of different types.");
	    return 1;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val;
	    int rval;
	    bionet_value_get_binary(other->this, &rval);
	    bionet_value_get_binary($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val;
	    uint8_t rval;
	    bionet_value_get_uint8(other->this, &rval);
	    bionet_value_get_uint8($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val;
	    int8_t rval;
	    bionet_value_get_int8(other->this, &rval);
	    bionet_value_get_int8($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val;
	    uint16_t rval;
	    bionet_value_get_uint16(other->this, &rval);
	    bionet_value_get_uint16($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val;
	    int16_t rval;
	    bionet_value_get_int16(other->this, &rval);
	    bionet_value_get_int16($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val;
	    uint32_t rval;
	    bionet_value_get_uint32(other->this, &rval);
	    bionet_value_get_uint32($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val;
	    int32_t rval;
	    bionet_value_get_int32(other->this, &rval);
	    bionet_value_get_int32($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val;
	    float rval;
	    bionet_value_get_float(other->this, &rval);
	    bionet_value_get_float($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val;
	    double rval;
	    bionet_value_get_double(other->this, &rval);
	    bionet_value_get_double($self->this, &val);
	    if (val > rval) {
		return 1;
	    } else if (val < rval) {
		return -1;
	    }
	    return 0;
	} 
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	{
	    char * val;
	    char * rval;
	    bionet_value_get_str(other->this, &rval);
	    bionet_value_get_str($self->this, &val);
	    return strncmp(val, rval, strlen(rval));
	}
	default:
	    g_warning("bionet-value.i: Unknown data type in comparison.");
	    return 0;
	}
    }

    Value * __div__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val /= (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val /= (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val /= (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val /= (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val /= (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val /= (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val /= (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val /= (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __div__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val /= (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val /= (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val /= (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val /= (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val /= (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val /= (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val /= (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val /= (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }
    Value * __div__(Value * other) {
	if ((bionet_resource_get_data_type(bionet_value_get_resource($self->this))) != (bionet_resource_get_data_type(bionet_value_get_resource(other->this)))) {
	    g_warning("bionet-value.i: Cannot add Values of different types.");
	    return NULL;
	}

	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t rval = 0;
	    bionet_value_get_uint8(other->this, &rval);
	    bionet_value_get_uint8($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t rval = 0;
	    bionet_value_get_int8(other->this, &rval);
	    bionet_value_get_int8($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t rval = 0;
	    bionet_value_get_uint16(other->this, &rval);
	    bionet_value_get_uint16($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t rval = 0;
	    bionet_value_get_int16(other->this, &rval);
	    bionet_value_get_int16($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t rval = 0;
	    bionet_value_get_uint32(other->this, &rval);
	    bionet_value_get_uint32($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t rval = 0;
	    bionet_value_get_int32(other->this, &rval);
	    bionet_value_get_int32($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float rval = 0;
	    bionet_value_get_float(other->this, &rval);
	    bionet_value_get_float($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double rval = 0;
	    bionet_value_get_double(other->this, &rval);
	    bionet_value_get_double($self->this, &val);
	    val /= rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __rdiv__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval / val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval / val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval / val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval / val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval / val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval / val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = (float)rval / val;
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = (double)rval / val;
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rdiv__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval / val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval / val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval / val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval / val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval / val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval / val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = (float)rval / val;
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = (double)rval / val;
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __mul__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val *= (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val *= (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val *= (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val *= (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val *= (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val *= (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val *= (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val *= (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __mul__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val *= (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val *= (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val *= (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val *= (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val *= (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val *= (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val *= (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val *= (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __mul__(Value * other) {
	if ((bionet_resource_get_data_type(bionet_value_get_resource($self->this))) != (bionet_resource_get_data_type(bionet_value_get_resource(other->this)))) {
	    g_warning("bionet-value.i: Cannot add Values of different types.");
	    return NULL;
	}

	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t rval = 0;
	    bionet_value_get_uint8(other->this, &rval);
	    bionet_value_get_uint8($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t rval = 0;
	    bionet_value_get_int8(other->this, &rval);
	    bionet_value_get_int8($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t rval = 0;
	    bionet_value_get_uint16(other->this, &rval);
	    bionet_value_get_uint16($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t rval = 0;
	    bionet_value_get_int16(other->this, &rval);
	    bionet_value_get_int16($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t rval = 0;
	    bionet_value_get_uint32(other->this, &rval);
	    bionet_value_get_uint32($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t rval = 0;
	    bionet_value_get_int32(other->this, &rval);
	    bionet_value_get_int32($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float rval = 0;
	    bionet_value_get_float(other->this, &rval);
	    bionet_value_get_float($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double rval = 0;
	    bionet_value_get_double(other->this, &rval);
	    bionet_value_get_double($self->this, &val);
	    val *= rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __rmul__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval * val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval * val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval * val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval * val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval * val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval * val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = (float)rval * val;
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = (double)rval * val;
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rmul__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval * val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval * val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval * val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval * val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval * val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval * val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = (float)rval * val;
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = (double)rval * val;
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __sub__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val -= (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val -= (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val -= (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val -= (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val -= (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val -= (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val -= (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val -= (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __sub__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val -= (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val -= (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val -= (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val -= (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val -= (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val -= (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val -= (float)rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val -= (double)rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __sub__(Value * other) {
	if ((bionet_resource_get_data_type(bionet_value_get_resource($self->this))) != (bionet_resource_get_data_type(bionet_value_get_resource(other->this)))) {
	    g_warning("bionet-value.i: Cannot add Values of different types.");
	    return NULL;
	}

	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t rval = 0;
	    bionet_value_get_uint8(other->this, &rval);
	    bionet_value_get_uint8($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t rval = 0;
	    bionet_value_get_int8(other->this, &rval);
	    bionet_value_get_int8($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t rval = 0;
	    bionet_value_get_uint16(other->this, &rval);
	    bionet_value_get_uint16($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t rval = 0;
	    bionet_value_get_int16(other->this, &rval);
	    bionet_value_get_int16($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t rval = 0;
	    bionet_value_get_uint32(other->this, &rval);
	    bionet_value_get_uint32($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t rval = 0;
	    bionet_value_get_int32(other->this, &rval);
	    bionet_value_get_int32($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float rval = 0;
	    bionet_value_get_float(other->this, &rval);
	    bionet_value_get_float($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double rval = 0;
	    bionet_value_get_double(other->this, &rval);
	    bionet_value_get_double($self->this, &val);
	    val -= rval;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __rsub__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval - val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval - val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval - val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval - val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval - val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval - val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = (float)rval - val;
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = (double)rval - val;
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rsub__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval - val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval - val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval - val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval - val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval - val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval - val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = (float)rval - val;
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = (double)rval - val;
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __neg__() {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val = -val;
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val = -val;
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val = -val;
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val = -val;
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val = -val;
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __pow__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __pow__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    bionet_value_get_uint8($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    bionet_value_get_int8($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    bionet_value_get_uint16($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    bionet_value_get_int16($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    bionet_value_get_uint32($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    bionet_value_get_int32($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    bionet_value_get_float($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    bionet_value_get_double($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __pow__(Value * other) {
	if ((bionet_resource_get_data_type(bionet_value_get_resource($self->this))) != (bionet_resource_get_data_type(bionet_value_get_resource(other->this)))) {
	    g_warning("bionet-value.i: Cannot add Values of different types.");
	    return NULL;
	}

	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t rval = 0;
	    bionet_value_get_uint8(other->this, &rval);
	    bionet_value_get_uint8($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t rval = 0;
	    bionet_value_get_int8(other->this, &rval);
	    bionet_value_get_int8($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int8(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t rval = 0;
	    bionet_value_get_uint16(other->this, &rval);
	    bionet_value_get_uint16($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t rval = 0;
	    bionet_value_get_int16(other->this, &rval);
	    bionet_value_get_int16($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int16(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t rval = 0;
	    bionet_value_get_uint32(other->this, &rval);
	    bionet_value_get_uint32($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_uint32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t rval = 0;
	    bionet_value_get_int32(other->this, &rval);
	    bionet_value_get_int32($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_int32(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float rval = 0;
	    bionet_value_get_float(other->this, &rval);
	    bionet_value_get_float($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_float(resource->this, val);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double rval = 0;
	    bionet_value_get_double(other->this, &rval);
	    bionet_value_get_double($self->this, &val);
	    val = pow((double)val, (double)rval);
	    value->this = bionet_value_new_double(resource->this, val);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
    }

    Value * __rpow__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rpow__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float val = 0;
	    float result;
	    bionet_value_get_float($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_float(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double val = 0;
	    double result;
	    bionet_value_get_double($self->this, &val);
	    result = pow((double)rval, (double)val);
	    value->this = bionet_value_new_double(resource->this, result);
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion from %s to integer.",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __and__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    int result;
	    bionet_value_get_binary($self->this, &val);
	    result = val & rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val & (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val & (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val & (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val & (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val & (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val & (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for '&' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rand__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    int result;
	    bionet_value_get_binary($self->this, &val);
	    result = val & rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val & (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val & (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val & (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val & (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val & (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val & (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for '&' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __or__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    int result;
	    bionet_value_get_binary($self->this, &val);
	    result = val | rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val | (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val | (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val | (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val | (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val | (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val | (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for '|' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __ror__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    int result;
	    bionet_value_get_binary($self->this, &val);
	    result = val | rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val | (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val | (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val | (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val | (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val | (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val | (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for '|' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __xor__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    int result;
	    bionet_value_get_binary($self->this, &val);
	    result = val ^ rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val ^ (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val ^ (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val ^ (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val ^ (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val ^ (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val ^ (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for '^' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rxor__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int val = 0;
	    int result;
	    bionet_value_get_binary($self->this, &val);
	    result = val ^ rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val ^ (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val ^ (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val ^ (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val ^ (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val ^ (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val ^ (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for '^' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __mod__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val % (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val % (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val % (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val % (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val % (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val % (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for 'mod' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __mod__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = val % (uint8_t)rval;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = val % (int8_t)rval;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = val % (uint16_t)rval;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = val % (int16_t)rval;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = val % (uint32_t)rval;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = val % (int32_t)rval;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for 'mod' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rmod__(int rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval % val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval % val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval % val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval % val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval % val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval % val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for 'mod' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

    Value * __rmod__(float rval) {
	Resource * resource = (Resource *)bionet_resource_get_user_data(bionet_value_get_resource($self->this));
	Value * value = (Value *)calloc(1, sizeof(Value));
	if (NULL == value) {
	    return NULL;
	}

	switch (bionet_resource_get_data_type(bionet_value_get_resource($self->this))) {

	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t val = 0;
	    uint8_t result;
	    bionet_value_get_uint8($self->this, &val);
	    result = (uint8_t)rval % val;
	    value->this = bionet_value_new_uint8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t val = 0;
	    int8_t result;
	    bionet_value_get_int8($self->this, &val);
	    result = (int8_t)rval % val;
	    value->this = bionet_value_new_int8(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t val = 0;
	    uint16_t result;
	    bionet_value_get_uint16($self->this, &val);
	    result = (uint16_t)rval % val;
	    value->this = bionet_value_new_uint16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t val = 0;
	    int16_t result;
	    bionet_value_get_int16($self->this, &val);
	    result = (int16_t)rval % val;
	    value->this = bionet_value_new_int16(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t val = 0;
	    uint32_t result;
	    bionet_value_get_uint32($self->this, &val);
	    result = (uint32_t)rval % val;
	    value->this = bionet_value_new_uint32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t val = 0;
	    int32_t result;
	    bionet_value_get_int32($self->this, &val);
	    result = (int32_t)rval % val;
	    value->this = bionet_value_new_int32(resource->this, result);
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	case BIONET_RESOURCE_DATA_TYPE_STRING:
	case BIONET_RESOURCE_DATA_TYPE_INVALID:
	default:
	    g_warning("bionet-value.i: Invalid datatype conversion for 'mod' operator: %s",
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(bionet_value_get_resource($self->this))));
	    free(value);
	    return NULL;
	}

	if ((NULL != value) && (NULL == value->this)) {
	    free(value);
	    value = NULL;
	} else if (NULL != value) {
	    bionet_value_set_user_data(value->this, value);
	}
	return value;
	
    }

}
