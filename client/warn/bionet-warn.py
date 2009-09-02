#!/usr/bin/python
#
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#

import sys
import optparse
import logging
import os
from select import select

def greater_than(a, b):
    return (a > b)

# parse options 
parser = optparse.OptionParser()

parser.add_option("-c", "--config", dest="config",
                  help="Configuration file",
                  default="config")

(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Warn")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)


# start the meat and potatoes
from bionet import *

jail = { }
try:
    execfile(options.config, jail)
except Exception, e:
    print e
    exit

if not 'warn' in jail:
    print options.config, "does not supply a node object, ignoring"
else:
    warn = jail['warn'];


#callbacks
def cb_datapoint(datapoint):
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);
    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);
    check = 0;

    for item in warn:
        if (item['resource'] == bionet_resource_get_name(resource)):
            check = 1;

    #not found, get outta here
    if (check == 0):
        return 0;

    #switch on the type
        #create a new value
        #populate the value
        #run the compare
            #execute the action
        #delete the value

    if (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_BINARY):
        val = new_binaryp();
        bionet_value_get_binary(value, val)
        if (item['compare'](binaryp_value(val))):
            os.system(item['command'])
        delete_binaryp(val)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_UINT8):
        val = new_uint8p();
        bionet_value_get_uint8(value, val)
        if (item['compare'](uint8p_value(val))):
            os.system(item['command'])
        delete_uint8p(val)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_INT8):
        val = new_int8p();
        bionet_value_get_int8(value, val)
        if (item['compare'](int8p_value(val))):
            os.system(item['command'])
        delete_int8p(val)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_UINT16):
        val = new_uint16p();
        bionet_value_get_uint16(value, val)
        if (item['compare'](uint16p_value(val))):
            os.system(item['command'])
        delete_uint16p(val)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_INT16):
        val = new_int16p();
        bionet_value_get_int16(value, val)
        if (item['compare'](int16p_value(val))):
            os.system(item['command'])
        delete_int16p(val)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_UINT32):
        val = new_uint32p();
        bionet_value_get_uint32(value, val)
        if (item['compare'](uint32p_value(val))):
            os.system(item['command'])
        delete_uint32p(val)

    elif (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_INT32):
        val = new_int32p();
        bionet_value_get_int32(value, val)
        if (item['compare'](int32p_value(val))):
            os.system(item['command'])
        delete_int32p(val)


# main
bionet_fd = bionet_connect()
if (0 > bionet_fd):
    logger.warning("error connecting to Bionet")
    exit(1)

pybionet_register_callback_datapoint(cb_datapoint);

#subscribe to all the requests
for item in warn:
    bionet_subscribe_datapoints_by_name(item['resource'])

while(1):
    (rr, wr, er) = select([bionet_fd], [], [])
    for fd in rr:
        bionet_read()
     
