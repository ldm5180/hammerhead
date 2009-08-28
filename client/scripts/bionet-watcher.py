#!/usr/bin/python
#
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


import sys
import optparse
import logging
from select import select

# parse options 
parser = optparse.OptionParser()
parser.add_option("-d", "--hab", "--habs", dest="hab_name",
                  help="Subscribe to a HAB list.", 
                  metavar="HAB-Type.HAB-ID")
parser.add_option("-n", "--node", "--nodes", dest="node_name",
                  help="Subscribe to a Node list.", 
                  metavar="HAB-Type.HAB-ID.Node-ID")
parser.add_option("-r", "--resource", "--resources", dest="resource_name",
                  help="Subscribe to a Resource list.", 
                  metavar="HAB-Type.HAB-ID.Node-ID:Resource-ID")
parser.add_option("-s", "--security-dir", dest="security_dir",
                  help="Directory containing security certificates.",
                  metavar="dir", default=None)
parser.add_option("-e", "--require-security", dest="require_security",
                  help="Require secured connections.",
                  action="store_true", default=False)

(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Watcher")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

# start the meat and potatoes
from bionet import *


#callbacks
def cb_lost_hab(hab):
    print("lost hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab))


def cb_new_hab(hab):
    print("new hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab))


def cb_new_node(node):
    hab = bionet_node_get_hab(node)
	
    print("new node: " + bionet_node_get_name(node))
	
    if (bionet_node_get_num_resources(node)):
        print("    Resources:")
	    
        for i in range(bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, i)
            datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
		
            if (datapoint == None):
                print("        " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + bionet_resource_get_id(resource) + ": (no known value)")
            
            else:
                value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
                #%s %s %s = %s @ %s 
                print("        " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + bionet_resource_get_id(resource) + " = " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint))

    if (bionet_node_get_num_streams(node)):
        print("    Streams:")
    
        for i in range(bionet_node_get_num_streams(node)):
            stream = bionet_node_get_stream_by_index(node, i)
            print("        " + bionet_stream_get_id(stream) + " " + bionet_stream_get_type(stream) + " " + bionet_stream_direction_to_string(bionet_stream_get_direction(stream)))


def cb_lost_node(node):
    hab = bionet_node_get_hab(node);
    print("lost node: " + bionet_node_get_name(node))


def cb_datapoint(datapoint):
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);
    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);
    
    value_str = bionet_value_to_str(value);
    #"%s.%s.%s:%s = %s %s %s @ %s"    
    print(bionet_resource_get_name(resource) + " = " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint))
    
if (options.security_dir != None):
    bionet_init_security(options.security_dir, options.require_security)

bionet_fd = bionet_connect()
if (0 > bionet_fd):
    logger.warning("error connecting to Bionet")
    exit(1)

logger.info("connected to Bionet")

pybionet_register_callback_new_hab(cb_new_hab)
pybionet_register_callback_lost_hab(cb_lost_hab);

pybionet_register_callback_new_node(cb_new_node);
pybionet_register_callback_lost_node(cb_lost_node);

pybionet_register_callback_datapoint(cb_datapoint);

subscribed_to_something = 0;

if (options.hab_name):
    bionet_subscribe_hab_list_by_name(options.hab_name)
    subscribed_to_something = 1;
if (options.node_name):
    bionet_subscribe_node_list_by_name(options.node_name)
    subscribed_to_something = 1;
if (options.resource_name):
    bionet_subscribe_datapoints_by_name(options.resource_name)
    subscribed_to_something = 1;

if (0 == subscribed_to_something):
    bionet_subscribe_hab_list_by_name("*.*");
    bionet_subscribe_node_list_by_name("*.*.*");
    bionet_subscribe_datapoints_by_name("*.*.*:*");

while(1):
     (rr, wr, er) = select([bionet_fd], [], [])
     for fd in rr:
         bionet_read()
    
