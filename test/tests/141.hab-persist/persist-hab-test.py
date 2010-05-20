#!/usr/bin/python

from hab import *
import logging
import optparse
import select
import time
import datetime
from ctypes import *

#set up logging
logger = logging.getLogger("Bionet Persist Test HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

#parse the incoming command line
parser = optparse.OptionParser()
parser.add_option("-i", "--id", dest="id", default=None,
                  help="HAB-ID",
                  metavar="HAB-ID")

(options,args) = parser.parse_args()

bionet_log_use_default_handler(None);

if hab_set_persist_directory("."):
    logger.error("Failed to set the persistency directory to this local dir")
    exit(1)

hab = bionet_hab_new("persist-test", options.id)
if (None == hab):
    logger.error("Failed to create HAB, exiting")
    exit(1)

hab_fd = hab_connect(hab)
if (0 > hab_fd):
    logger.warning("problem connecting HAB to Bionet, exiting")
    exit(1)

node = bionet_node_new(hab, "test")
if (None == node):
    logger.error("Failed to create node, exiting")
    exit(1)
bionet_hab_add_node(hab, node)

# binary 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_BINARY, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "binary");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_binary(resource, 0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# uint8
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_UINT8, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint8");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_uint8(resource, 0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# int8 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_INT8, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "int8");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_int8(resource, 0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# uint16 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_UINT16, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint16");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_uint16(resource, 0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# int16 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_INT16, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "int16");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_int16(resource, 0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# uint32 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint32");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_uint32(resource, 0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# int32 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_INT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "int32");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_int32(resource, 0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# float 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_FLOAT, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "float");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_float(resource, 0.0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# double 
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "double");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_double(resource, 0.0, None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# string
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_STRING, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "string");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_str(resource, "0", None)
value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                value_str,
                                bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

# report stuff
hab_report_new_node(node)
hab_report_datapoints(node)

i = 0
next_pub = 0
starttime = time.time()
while(time.time() - starttime < 30):
    (rr, wr, er) = select.select([hab_fd], [], [], 1.0)
    if (rr):
        hab_read()

    i += 1

    # for the first 5 seconds, do not publish new datapoints, wait for clients
    if (5 > time.time() - starttime):
        continue;

    if (next_pub <= time.time()):
        next_pub = time.time() + 1
    else: 
        continue

    resource = bionet_node_get_resource_by_id(node, "binary")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_binary(resource, int(time.time() % 2), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "uint8")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_uint8(resource, int(time.time() % 0xFF), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "int8")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_int8(resource, int(time.time() % 0x7F), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "uint16")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_uint16(resource, int(time.time() % 0xFFFF), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "int16")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_int16(resource, int(time.time() % 0x7FFF), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "uint32")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_uint32(resource, int(time.time()), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "int32")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_int32(resource, int(time.time()), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "float")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_float(resource, float(time.time()/1000000.0), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "double")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_double(resource, float(time.time()/1000000.0), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    resource = bionet_node_get_resource_by_id(node, "string")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    bionet_resource_set_str(resource, str(time.time()), None)
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    #report stuff
    hab_report_datapoints(node)

time.sleep(1)
hab_read()
exit(0)
