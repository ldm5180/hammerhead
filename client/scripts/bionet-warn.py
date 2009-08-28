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

parser.add_option("-r", "--resource", "--resources", dest="resource_name",
                  help="Subscribe to a Resource list.", 
                  metavar="HAB-Type.HAB-ID.Node-ID:Resource-ID")

parser.add_option("-s", "--min", "--small", dest="minimum",
                  help="Minimum value of resource.",
                  default=None)

parser.add_option("-b", "--max", "--big", dest="maximum",
                  help="Maximum value of resource.",
                  default=None)

parser.add_option("-m", "--match", "--equal", dest="match",
                  help="Match value of resource.",
                  default=None)

parser.add_option("-n", "--not", "--unequal", dest="not_eq",
                  help="Do not match value of resource.",
                  default=None)

parser.add_option("-e", "--email", dest="email",
                  help="Email address to alert.",
                  default=None)


(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Warn")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

if ((options.email == None) or (options.resource_name == None)):
    print ("Resource name and email must both be defined.")
    exit(1)

if ((options.minimum == None) and (options.maximum == None) and (options.match == None) and (options.not_eq == None)):
    print ("At least one qualifier needs to be defined")
    exit(1)

# start the meat and potatoes
from bionet import *

#callbacks
def cb_datapoint(datapoint):
    #check the datapoint and mail if it is bad!
    value = bionet_datapoint_get_value(datapoint);

    if (options.minimum != None):
        print "Checking minimum"
        None

    if (options.maximum != None):
        print "Checking maximum"
        None

    if (options.match != None):
        print "Checking match"
        None

    if (options.not_eq != None):
        print "Checking not"
        None


# main
bionet_fd = bionet_connect()
if (0 > bionet_fd):
    logger.warning("error connecting to Bionet")
    exit(1)

pybionet_register_callback_datapoint(cb_datapoint);

if (options.resource_name):
    bionet_subscribe_datapoints_by_name(options.resource_name)
    subscribed_to_something = 1;

while(1):
     (rr, wr, er) = select([bionet_fd], [], [])
     for fd in rr:
         bionet_read()
