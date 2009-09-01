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
    
    value_str = bionet_value_to_str(value);
    print "TODO: validate", bionet_resource_get_name(resource) + " = " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint) 


# main
bionet_fd = bionet_connect()
if (0 > bionet_fd):
    logger.warning("error connecting to Bionet")
    exit(1)

pybionet_register_callback_datapoint(cb_datapoint);

for item in warn:
    bionet_subscribe_datapoints_by_name(item['resource']);


while(1):
    (rr, wr, er) = select([bionet_fd], [], [])
    for fd in rr:
        bionet_read()
     
