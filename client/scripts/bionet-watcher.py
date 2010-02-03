#!/usr/bin/python
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


import sys
import optparse
import logging
from select import select
import time

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
parser.add_option("--bdm-only", dest="bdm_only",
                  help="Subscribe to BDMs only.",
                  action="store_true", default=False)
parser.add_option("-T", "--datapoint-start", dest="datapoint_start",
                  help="Datapoint Start Time for BDM subscriptions", 
                  metavar="YYYY-MM-DD HH:MM:SS")
parser.add_option("-t", "--datapoint-end", dest="datapoint_end",
                  help="Datapoint End Time for BDM subscriptions", 
                  metavar="YYYY-MM-DD HH:MM:SS")

(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Watcher")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

# since bionet wants times in UTC...
def gmt_convert(loctime):
    if (time.daylight):
        return loctime - time.altzone
    else:
        return loctime - time.timezone


from bionet import *
from bdm_client import *

if (options.datapoint_start != None):
    datapoint_start = timeval()
    datapoint_start.tv_sec = gmt_convert(int(time.mktime(time.strptime(options.datapoint_start, 
                                                             "%Y-%m-%d %H:%M:%S"))))
    datapoint_start.tv_usec = 0
else:
    datapoint_start = None

if (options.datapoint_end != None):
    datapoint_end = timeval()
    datapoint_end.tv_sec = gmt_convert(int(time.mktime(time.strptime(options.datapoint_end, 
                                                             "%Y-%m-%d %H:%M:%S"))))
    datapoint_end.tv_usec = 0
else:
    datapoint_end = None

if (options.bdm_only) and (datapoint_start == None) and (datapoint_end == None):
    logger.error("A datapoint start and/or end time must be specified when in BDM-Only mode")
    exit(1)

# start the meat and potatoes

#bionet callbacks
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


#bdm callbacks
def cb_bdm_lost_hab(hab):
    print("BDM lost hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab))


def cb_bdm_new_hab(hab):
    print("BDM new hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab))


def cb_bdm_new_node(node):
    hab = bionet_node_get_hab(node)
	
    print("BDM new node: " + bionet_node_get_name(node))
	
    if (bionet_node_get_num_resources(node)):
        print("    BDM Resources:")
	    
        for i in range(bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, i)
            datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
		
            if (datapoint == None):
                print("        BDM " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + bionet_resource_get_id(resource) + ": (no known value)")
            
            else:
                value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
                #%s %s %s = %s @ %s 
                print("        BDM " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + bionet_resource_get_id(resource) + " = " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint))

    if (bionet_node_get_num_streams(node)):
        print("    BDM Streams:")
    
        for i in range(bionet_node_get_num_streams(node)):
            stream = bionet_node_get_stream_by_index(node, i)
            print("        BDM " + bionet_stream_get_id(stream) + " " + bionet_stream_get_type(stream) + " " + bionet_stream_direction_to_string(bionet_stream_get_direction(stream)))


def cb_bdm_lost_node(node):
    hab = bionet_node_get_hab(node);
    print("BDM lost node: " + bionet_node_get_name(node))


def cb_bdm_datapoint(datapoint):
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);
    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);
    
    value_str = bionet_value_to_str(value);
    #"%s.%s.%s:%s = %s %s %s @ %s"    
    print("BDM " + bionet_resource_get_name(resource) + " = " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint))

    
if (options.security_dir != None):
    bionet_init_security(options.security_dir, options.require_security)

bdm_fd = -1
bionet_fd = -1

if (options.bdm_only == False):
    bionet_fd = bionet_connect()
    if (0 > bionet_fd):
        logger.error("error connecting to Bionet")
        exit(1)
    else:
        logger.info("connected to Bionet")

if (datapoint_start != None) or (datapoint_end != None):
    bdm_fd = bdm_start() 
    if (bdm_fd < 0):
        logger.error("Failed to connect to Bionet Data Manager.")
        exit(1)
    else:
        logger.info("connected to Bionet Data Manager")


if (bionet_fd == -1) and (bdm_fd == -1):
    logger.error("Failed to connect to both Bionet and Bionet Data Manager.")
    sys.exit(1)

# register Bionet callbacks
if (bionet_fd != -1):
    pybionet_register_callback_new_hab(cb_new_hab)
    pybionet_register_callback_lost_hab(cb_lost_hab);
    pybionet_register_callback_new_node(cb_new_node);
    pybionet_register_callback_lost_node(cb_lost_node);
    pybionet_register_callback_datapoint(cb_datapoint);

    bionet_subscribed_to_something = 0;

    if (options.hab_name):
        bionet_subscribe_hab_list_by_name(options.hab_name)
        bionet_subscribed_to_something = 1;
    if (options.node_name):
        bionet_subscribe_node_list_by_name(options.node_name)
        bionet_subscribed_to_something = 1;
    if (options.resource_name):
        bionet_subscribe_datapoints_by_name(options.resource_name)
        bionet_subscribed_to_something = 1;

    if (0 == bionet_subscribed_to_something):
        bionet_subscribe_hab_list_by_name("*.*");
        bionet_subscribe_node_list_by_name("*.*.*");
        bionet_subscribe_datapoints_by_name("*.*.*:*");


# register BDM callbacks
if (bdm_fd != -1):
    pybdm_register_callback_new_hab(cb_bdm_new_hab, None)
    pybdm_register_callback_lost_hab(cb_bdm_lost_hab, None);
    pybdm_register_callback_new_node(cb_bdm_new_node, None);
    pybdm_register_callback_lost_node(cb_bdm_lost_node, None);
    pybdm_register_callback_datapoint(cb_bdm_datapoint, None);

    bdm_subscribed_to_something = 0;

    if (options.hab_name):
        bdm_subscribe_hab_list_by_name(options.hab_name)
        bdm_subscribed_to_something = 1;
    if (options.node_name):
        bdm_subscribe_node_list_by_name(options.node_name)
        bdm_subscribed_to_something = 1;
    if (options.resource_name):
        bdm_subscribe_datapoints_by_name(options.resource_name, datapoint_start, datapoint_end)
        bdm_subscribed_to_something = 1;

    if (0 == bdm_subscribed_to_something):
        bdm_subscribe_hab_list_by_name("*.*");
        bdm_subscribe_node_list_by_name("*.*.*");
        bdm_subscribe_datapoints_by_name("*.*.*:*", datapoint_start, datapoint_end);

fd_list = []
if (bionet_fd != -1):
    fd_list.append(bionet_fd)
if (bdm_fd != -1):
    fd_list.append(bdm_fd)

while(1):
     (rr, wr, er) = select(fd_list, [], [])
     for fd in rr:
         if (fd == bionet_fd):
             #logger.info("Reading from Bionet...");
             bionet_read()
         if (fd == bdm_fd):
             #logger.info("Reading from BDM...");
             bdm_read()
