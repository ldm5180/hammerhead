#!/usr/bin/python
#
# Copyright (c) 2010, Regents of the University of Colorado.
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
parser.add_option("-r", "--resource", "--resources", dest="resource_name",
                  help="Subscribe to a Resource list.", 
                  metavar="HAB-Type.HAB-ID.Node-ID:Resource-ID")
parser.add_option("-s", "--security-dir", dest="security_dir",
                  help="Directory containing security certificates.",
                  metavar="dir", default=None)
parser.add_option("-e", "--require-security", dest="require_security",
                  help="Require secured connections.",
                  action="store_true", default=False)
parser.add_option("-t", "--time-to-run", dest="time_to_run",
                  help="Duration to check", default="60")

(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Highest BDM Performance")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

from bionet import *

global highest
highest = 0

#bionet callbacks
def cb_lost_hab(hab):
    None


def cb_new_hab(hab):
    None


def cb_new_node(node):
    global highest
    hab = bionet_node_get_hab(node)
    if (bionet_node_get_num_resources(node)):
        for i in range(bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, i)
            datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
            value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint))
            val = int(value_str);
            if (val > highest):
                highest = val

def cb_lost_node(node):
    None

def cb_datapoint(datapoint):
    global highest
    value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint))
    val = int(value_str);
    if (val > highest):
        highest = val

if (options.security_dir != None):
    bionet_init_security(options.security_dir, options.require_security)

bionet_fd = -1
bionet_fd = bionet_connect()
if (0 > bionet_fd):
    logger.error("error connecting to Bionet")
    exit(1)
else:
    # register Bionet callbacks
    pybionet_register_callback_new_hab(cb_new_hab)
    pybionet_register_callback_lost_hab(cb_lost_hab);
    pybionet_register_callback_new_node(cb_new_node);
    pybionet_register_callback_lost_node(cb_lost_node);
    pybionet_register_callback_datapoint(cb_datapoint);

if (None == options.resource_name):
    logger.error("Resource name needs to be specified.")
    exit(1)

bionet_subscribe_datapoints_by_name(options.resource_name)

fd_list = []
if (bionet_fd != -1):
    fd_list.append(bionet_fd)

started = time.time()
while(1):
    (rr, wr, er) = select(fd_list, [], [], 1)
    for fd in rr:
        if (fd == bionet_fd):
            bionet_read()
    if ((time.time() - started) >= float(options.time_to_run)):
        print "Bionet Data Manager max speed:", highest, "datapoints/sec"
        exit(0)
