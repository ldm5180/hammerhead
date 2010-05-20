#!/usr/bin/python

from hab import *
import logging
import optparse
import select
import time
import datetime

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

# uint32 to be incremented every 1 sec
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint32-1");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_uint32(resource, 0, None)

# uint32 to be incremented every 5 sec
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint32-5");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_uint32(resource, 0, None)

# float to be incremented every 1 sec
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_FLOAT, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "float-1");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_float(resource, 0.0, None)

# float to be incremented every 5 sec
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_FLOAT, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "float-5");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_float(resource, 0.0, None)

# string to be incremented every 1 sec
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_STRING, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "string-1");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_str(resource, "0", None)

# float to be incremented every 5 sec
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_STRING, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "string-5");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node, resource)
if (hab_persist_resource(resource)):
    logger.warning("Not persisting resource %s" % bionet_resource_get_name(resource))
if (None == bionet_resource_get_datapoint_by_index(resource, 0)):
    bionet_resource_set_str(resource, "0", None)

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


    resource = bionet_node_get_resource_by_id(node, "uint32-1")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    newval = int(val) + 1;
    bionet_resource_set_uint32(resource, newval, None)
    logger.debug("uint32-1 = %d" % newval)

    resource = bionet_node_get_resource_by_id(node, "float-1")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    newval = int(val) + 1;
    bionet_resource_set_float(resource, float(newval), None)
    logger.debug("float-1 = %d" % newval)

    resource = bionet_node_get_resource_by_id(node, "string-1")
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    newval = int(val) + 1;
    bionet_resource_set_str(resource, str(newval), None)
    logger.debug("string-1 = %d" % newval)


    if (0 == i % 5):
        resource = bionet_node_get_resource_by_id(node, "uint32-5")
        val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
        newval = int(val) + 1;
        bionet_resource_set_uint32(resource, newval, None)
        logger.debug("uint32-5 = %d" % newval)
        
        resource = bionet_node_get_resource_by_id(node, "float-5")
        val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
        newval = int(val) + 1;
        bionet_resource_set_float(resource, float(newval), None)
        logger.debug("float-5 = %d" % newval)
        
        resource = bionet_node_get_resource_by_id(node, "string-5")
        val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
        newval = int(val) + 1;
        bionet_resource_set_str(resource, str(newval), None)
        logger.debug("string-1 = %d" % newval)

    hab_report_datapoints(node)

time.sleep(1)
hab_read()
exit(0)
