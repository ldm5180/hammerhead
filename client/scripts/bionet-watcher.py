#!/usr/bin/python

import sys
import optparse
import logging

# parse options 
parser = optparse.OptionParser()
parser.add_option("-d", "--hab", "--habs", dest="hab_name",
                  help="Subscribe to a HAB list.", 
                  metavar="HAB-Type.HAB-ID")
parser.add_option("-n", "--node", "--nodes", dest="node_name",
                  help="Subscribe to a Node list.", 
                  metavar="HAB-Type.HAB-ID.Node-ID")
parser.add_option("-r", "--resource", "--resources", dest="resource_name",
                  help="Subscribe to a Node list.", 
                  metavar="HAB-Type.HAB-ID.Node-ID:Resource-ID")

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



bionet_fd = bionet_connect()
if (0 > bionet_fd):
    logger.warning("error connecting to Bionet")
    exit(1)

logger.info("connected to Bionet")

bionet_register_callback_new_hab(cb_new_hab)
bionet_register_callback_lost_hab(cb_lost_hab);

bionet_register_callback_new_node(cb_new_node);
bionet_register_callback_lost_node(cb_lost_node);

bionet_register_callback_datapoint(cb_datapoint);

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
    bionet_read()
