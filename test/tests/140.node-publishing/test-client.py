#!/usr/bin/python

from bionet import *
from time import *
import optparse 
import sys
import select

parser = optparse.OptionParser()
parser.add_option("-i", "--id", dest="id",
                  help="HAB ID to subscribe.", 
                  metavar="HAB-ID", default=None)
(options, args) = parser.parse_args()

if options.id == None:
    print "Please subscribe to and id."
    sys.exit(1)

def cb_datapoint(datapoint):
    value = bionet_datapoint_get_value(datapoint)
    resource = bionet_value_get_resource(value)
    if (bionet_resource_get_id(resource) == "test-resource"):
        valstr = bionet_value_to_str(value)
        if (valstr == "0"):
            sys.exit(0)

    

pybionet_register_callback_datapoint(cb_datapoint);

bionet_fd = bionet_connect()

resname = "test-hab." + options.id + ".test-node:test-resource"
bionet_subscribe_datapoints_by_name(resname)

count = 0
started = time()
fds = [bionet_fd]
while (time() - started < 10):
    inputready,outputready,exceptready = select.select(fds,[],[], 1)
    bionet_read()

sys.exit(2)
    
