#!/usr/bin/python

from hab import *
import logging
import optparse
import select

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
while(i <= 30):
    (rr, wr, er) = select.select([hab_fd], [], [], 1.0)
    if (rr):
        hab_read()

    i += 1

    resource = bionet_node_get_resource_by_id(node, "uint32-1")
    bionet_resource_set_uint32(resource, i, None)

    resource = bionet_node_get_resource_by_id(node, "float-1")
    bionet_resource_set_float(resource, float(i), None)

    resource = bionet_node_get_resource_by_id(node, "string-1")
    bionet_resource_set_str(resource, str(i), None)

    logger.debug("1 = %d" % i)

    if (0 == i % 5):
        resource = bionet_node_get_resource_by_id(node, "uint32-5")
        bionet_resource_set_uint32(resource, i/5, None)
        
        resource = bionet_node_get_resource_by_id(node, "float-5")
        bionet_resource_set_float(resource, float(i/5), None)
        
        resource = bionet_node_get_resource_by_id(node, "string-5")
        bionet_resource_set_str(resource, str(i/5), None)

    logger.debug("5 = %d" % int(i/5))

    hab_report_datapoints(node)

time.sleep(1)
hab_read()
exit(0)
