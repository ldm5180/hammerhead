
from hab import *
import random
import logging
from ctypes import *
import sys

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

def Set(resource):
    if (None == resource):
        return

    if (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_INVALID):
        logger.warning("*** INVALID DATA TYPE\n")

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_BINARY):
        bionet_resource_set_binary(resource, random.randint(0,1), None)
        
    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_UINT8):
        ui8val = random.randint(0,255)
        bionet_resource_set_uint8(resource, ui8val, None)
        
    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_INT8):
        i8val = random.randint(-127,127)
        bionet_resource_set_int8(resource, i8val, None)
        
    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_UINT16):
        ui16val = random.randint(0,65535)
        bionet_resource_set_uint16(resource, ui16val, None)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_INT16):
        i16val = random.randint(-32767,32767)
        bionet_resource_set_int16(resource, i16val, None)
        
    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_UINT32):
        ui32val = random.randint(-4294967295,4294967295)
        bionet_resource_set_uint32(resource, ui32val, None)
        
    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_INT32):
        i32val = random.randint(-2147483647,2147483647)
        bionet_resource_set_int32(resource, i32val, None)
        
    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_FLOAT):
        fval = random.random()
        bionet_resource_set_float(resource, fval, None)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_DOUBLE):
        dval = random.random()
        bionet_resource_set_double(resource, dval, None)


    # FIXME: this one needs a special case
    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_STRING):
        new_string[0] = '\0'
        num_words = random.randint(0,3) + 1
        for i in range (num_words):
            new_string = new_string + get_random_word() + " "
            bionet_resource_set_str(resource, new_string, None)
            return
                
#    else:
#        logger.debug("*** unknown data type, skipping")



