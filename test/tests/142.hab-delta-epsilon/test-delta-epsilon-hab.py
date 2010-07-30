#!/usr/bin/python

from hab import *
import logging
import optparse
import select
import time
import datetime
from ctypes import *

#set up logging
logger = logging.getLogger("Bionet Delta/Epsilon Test HAB")
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

hab = bionet_hab_new("delta-epsilon-test", options.id)
if (None == hab):
    logger.error("Failed to create HAB, exiting")
    exit(1)

hab_fd = hab_connect(hab)
if (0 > hab_fd):
    logger.warning("problem connecting HAB to Bionet, exiting")
    exit(1)



node_d1 = bionet_node_new(hab, "delta-1")
if (None == node_d1):
    logger.error("Failed to create node, exiting")
    exit(1)
bionet_hab_add_node(hab, node_d1)

# uint32 
resource = bionet_resource_new(node_d1, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint32-count");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node_d1, resource)
delta = timeval()
delta.tv_sec = 1
delta.tv_usec = 0
if (bionet_resource_set_delta(resource, delta)):
    logger.warning("Not setting delta/epsilon on resource %s" % bionet_resource_get_name(resource))

# report stuff
hab_report_new_node(node_d1)
hab_report_datapoints(node_d1)



node_d5 = bionet_node_new(hab, "delta-5")
if (None == node_d5):
    logger.error("Failed to create node, exiting")
    exit(1)
bionet_hab_add_node(hab, node_d5)

# uint32 
resource = bionet_resource_new(node_d5, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint32-count");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node_d5, resource)
delta = timeval()
delta.tv_sec = 5
delta.tv_usec = 0
if (bionet_resource_set_delta(resource, delta)):
    logger.warning("Not setting delta/epsilon on resource %s" % bionet_resource_get_name(resource))

# report stuff
hab_report_new_node(node_d5)
hab_report_datapoints(node_d5)



node_e1 = bionet_node_new(hab, "epsilon-1")
if (None == node_e1):
    logger.error("Failed to create node, exiting")
    exit(1)
bionet_hab_add_node(hab, node_e1)

# uint32 
resource = bionet_resource_new(node_e1, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint32-count");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node_e1, resource)
epsilon = bionet_epsilon_new_uint32(1)
delta = timeval()
delta.tv_sec = 0
delta.tv_usec = 0
if (bionet_resource_set_epsilon(resource, epsilon)):
    logger.warning("Not setting delta/epsilon on resource %s" % bionet_resource_get_name(resource))

# report stuff
hab_report_new_node(node_e1)
hab_report_datapoints(node_e1)



node_e5 = bionet_node_new(hab, "epsilon-5")
if (None == node_e5):
    logger.error("Failed to create node, exiting")
    exit(1)
bionet_hab_add_node(hab, node_e5)

# uint32 
resource = bionet_resource_new(node_e5, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "uint32-count");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(node_e5, resource)
epsilon = bionet_epsilon_new_uint32(5)
delta = timeval()
delta.tv_sec = 0
delta.tv_usec = 0
if (bionet_resource_set_epsilon(resource, epsilon)):
    logger.warning("Not setting delta/epsilon on resource %s" % bionet_resource_get_name(resource))

# report stuff
hab_report_new_node(node_e5)
hab_report_datapoints(node_e5)


generic = bionet_node_new(hab, "generic")
if (None == generic):
    logger.error("Failed to create node, exiting")
    exit(1)
bionet_hab_add_node(hab, generic)

# uint32 
resource = bionet_resource_new(generic, 
                               BIONET_RESOURCE_DATA_TYPE_UINT32, 
                               BIONET_RESOURCE_FLAVOR_SENSOR,
                               "generic-count");
if (None == resource):
    logger.error("Failed to create resource, exiting")
    exit(1)
bionet_node_add_resource(generic, resource)

# report stuff
hab_report_new_node(generic)
hab_report_datapoints(generic)



i = 0
next_pub = 0
starttime = time.time()
while(time.time() - starttime < 30):
    (rr, wr, er) = select.select([hab_fd], [], [], 1.0)
#    (rr, wr, er) = select.select([hab_fd], [], [], 0.00001)
    if (rr):
        hab_read()

    # for the first 5 seconds, do not publish new datapoints, wait for clients
    if (5 > time.time() - starttime):
        continue;

    if (next_pub <= time.time()):
        next_pub = time.time() + 1
    else: 
        continue

    resource = bionet_node_get_resource_by_id(node_d1, "uint32-count")
    bionet_resource_set_uint32(resource, int(time.time()), None)
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    #report stuff
    hab_report_datapoints(node_d1)


    resource = bionet_node_get_resource_by_id(node_d5, "uint32-count")
    bionet_resource_set_uint32(resource, int(time.time()), None)
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    #report stuff
    hab_report_datapoints(node_d5)


    resource = bionet_node_get_resource_by_id(node_e1, "uint32-count")
    bionet_resource_set_uint32(resource, int(time.time()), None)
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    #report stuff
    hab_report_datapoints(node_e1)


    resource = bionet_node_get_resource_by_id(node_e5, "uint32-count")
    bionet_resource_set_uint32(resource, int(time.time()), None)
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    #report stuff
    hab_report_datapoints(node_e5)


    resource = bionet_node_get_resource_by_id(generic, "generic-count")
    bionet_resource_set_uint32(resource, int(time.time()), None)
    val = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)))
    value_str = bionet_value_to_str(bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0)));
    print ( "%s = %s %s %s @ %s" % (bionet_resource_get_name(resource),
                                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                                    value_str,
                                    bionet_datapoint_timestamp_to_string(bionet_resource_get_datapoint_by_index(resource, 0))))

    #report stuff
    hab_report_datapoints(generic)

hab_read()
exit(0)
