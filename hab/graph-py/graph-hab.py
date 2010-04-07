#!/usr/bin/python
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


import sys
import optparse
import logging
import time
import random

parser = optparse.OptionParser()
parser.add_option("-i", "--id", dest="hab_id", default="python",
                  help="Use ID as the HAB-ID (defaults to hostname if omitted)",
                  metavar="ID")
parser.add_option("-n", "--min-nodes", dest="min_nodes", default=5,
                  help="If there are fewer than N nodes, make up some more.",
                  metavar="N")
parser.add_option("-x", "--max-delay", dest="max_delay",
                  help="After taking each action (adding or removing a Node, or updating a Resource), the random-hab sleeps up to this long (seconds)",
                  default=1,
                  metavar="X")
parser.add_option("-t", "--test", dest="test", default=None,
                  help="Output all data to a file formatted for testing against BDM.",
                  metavar="FILE")
parser.add_option("-l", "--loop", dest="loops", default=0,
                  help="Number of times to do node updates before quitting.")

(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Graph HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)


from hab import *

userFloat = 1.0

def bionet_resource_set_value(resource, value):
    global userFloat
    dp = bionet_resource_get_datapoint_by_index(resource, 0)
    newVal = bionet_value_dup(resource, value)
    bionet_datapoint_set_value(dp, newVal)
    # Special handle float
    type = bionet_resource_get_data_type(resource)
    valstr = bionet_value_to_str(newVal)
    if type == BIONET_RESOURCE_DATA_TYPE_FLOAT:
	userFloat = float(valstr)

def pycb_set_resource(resource, value):
    print "callback: setting " + bionet_resource_get_local_name(resource) + " to " + bionet_value_to_str(value)
    bionet_resource_set_value(resource, value)


def print_val(resource):
    datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
    value = bionet_datapoint_get_value(datapoint)
    logger.info("    " + bionet_resource_get_id(resource) + " " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " = " + bionet_value_to_str(bionet_datapoint_get_value(datapoint)))


#connect to bionet
hab = bionet_hab_new("GRAPH", options.hab_id)
pyhab_register_callback_set_resource(pycb_set_resource);
bionet_fd = hab_connect(hab)
if (0 > bionet_fd):
    logger.warning("problem connection to Bionet, exiting\n")
    exit(1)

loops = 0

#test mode. open the output file and sleep to let subscribers catch up
if (options.test):
    f = open(options.test, "w")
    time.sleep(10)
else:
    f = None;


hab_read()

## make nodes
# Random graph
node = bionet_node_new(hab, "random")

#Add Resource
resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_FLOAT, 0, "plot")
bionet_node_add_resource(node, resource)
bionet_resource_set_float(resource, 0.0, None)
hab = bionet_node_get_hab(node)

if (bionet_hab_add_node(hab, node) != 0):
	logger.warning("HAB failed to add Node")

hab_report_new_node(node)

# Writable Params
nodeW = bionet_node_new(hab, "write")

#Add Resource
resourceW = bionet_resource_new(nodeW, BIONET_RESOURCE_DATA_TYPE_FLOAT, 1, "float")
bionet_node_add_resource(nodeW, resourceW)
bionet_resource_set_float(resourceW, 0.0, None)

resourceS = bionet_resource_new(nodeW, BIONET_RESOURCE_DATA_TYPE_STRING, 1, "string")
bionet_node_add_resource(nodeW, resourceS)
bionet_resource_set_str(resourceS, "", None)

if (bionet_hab_add_node(hab, nodeW) != 0):
	logger.warning("HAB failed to add Node")

hab_report_new_node(node)
while (int(options.loops) == 0) or (loops < int(options.loops)):
    global userFloat
    dval = random.random()
    bionet_resource_set_float(resource, dval, None)
    bionet_resource_set_float(resourceW, userFloat + dval, None)
    print '%(f)f' % {'f': userFloat}
    print_val(resourceW)

    hab_report_datapoints(node)
    hab_report_datapoints(nodeW)
    hab_read()

    loops = loops + 1
    time.sleep(options.max_delay)

#
# vim: ts=8 sw=4 sta
#
