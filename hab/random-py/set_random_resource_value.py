
from hab import *
import random
import logging

logger = logging.getLogger("Bionet Random HAB")

def randomize_buffer(buffer, size):
    for i in range(0,size):
        buffer[i] = random.randint(0,255) & 0xFF;


def Set(resource):
    value = new bionet_datapoint_value_t
    
    if (resource.data_type == BIONET_RESOURCE_DATA_TYPE_INVALID):
        logger.warning("*** INVALID DATA TYPE\n")

    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_BINARY):
        value.binary_v = 0
        value.binary_v = random.randint(0,1)
        
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_UINT8):
        value.uint8_v = 0
        randomize_buffer(value.uint8_v, sizeof(uint8_t))
        
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_INT8):
        value.int8_v = 0
        randomize_buffer(value.int8_v, sizeof(int8_t))
        
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_UINT16):
        value.uint16_v = 0
        randomize_buffer(value.uint16_v, sizeof(uint16_t))
        
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_INT16):
        value.int16_v = 0
        randomize_buffer(value.int16_v, sizeof(int16_t))
        
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_UINT32):
        value.uint32_v = 0
        randomize_buffer(value.uint32_v, sizeof(uint32_t))
        
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_INT32):
        value.int32_v = 0
        randomize_buffer(value.int32_v, sizeof(int32_t))
        
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_FLOAT):
        value.float_v = 0
        randomize_buffer(value.float_v, sizeof(float))
            
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_DOUBLE):
        value.double_v = 0
        randomize_buffer(value.double_v, sizeof(double))
            
    # FIXME: this one needs a special case
    elif (resource.data_type == BIONET_RESOURCE_DATA_TYPE_STRING):
        new_string[0] = '\0'
        num_words = random.randint(0,3) + 1
        for i in range (num_words):
            new_string = new_string + get_random_word() + " "
        value.string_v = new_string
        if (value.string_v == None):
            logger.error("*** out of memory!")
            return
                
    else:
        logger.debug("*** unknown data type, skipping")

    datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
    if (datapoint == None):
        datapoint = bionet_datapoint_new(resource, value, None)
        bionet_resource_add_existing_datapoint(resource, datapoint)
    else:
        bionet_datapoint_set_value(datapoint, value)
        bionet_datapoint_set_timestamp(datapoint, None)


