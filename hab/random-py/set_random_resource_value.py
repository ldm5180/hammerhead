
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


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

    if (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_INVALID):
        logger.warning("*** INVALID DATA TYPE\n")

    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_BINARY):
        resource.set(random.randint(0,1))
        
    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_UINT8):
        ui8val = random.randint(0,255)
        resource.set(ui8val)
        
    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_INT8):
        i8val = random.randint(-127,127)
        resource.set(i8val)
        
    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_UINT16):
        ui16val = random.randint(0,65535)
        resource.set(ui16val)

    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_INT16):
        i16val = random.randint(-32767,32767)
        resource.set(i16val)
        
    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_UINT32):
        ui32val = random.randint(-4294967295,4294967295)
        resource.set(ui32val)
        
    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_INT32):
        i32val = random.randint(-2147483647,2147483647)
        resource.set(i32val)
        
    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_FLOAT):
        fval = random.random()
        resource.set(fval)

    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_DOUBLE):
        dval = random.random()
        resource.set(dval)


    # FIXME: this one needs a special case
    elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_STRING):
        new_string[0] = '\0'
        num_words = random.randint(0,3) + 1
        for i in range (num_words):
            new_string = new_string + get_random_word() + " "
            resource.set(new_string)
            return
                
#    else:
#        logger.debug("*** unknown data type, skipping")



