#!/usr/bin/python

from hab import *
from time import *
import sys
import select
import optparse

parser = optparse.OptionParser()
parser.add_option("-i", "--id", dest="id",
                  help="HAB ID to subscribe.", 
                  metavar="HAB-ID", default=None)
(options, args) = parser.parse_args()

if options.id == None:
    print "Please subscribe to and id."
    sys.exit(1)

#make the hab
hab = bionet_hab_new("test-hab", options.id)

#connect to bionet
hab_fd = hab_connect(hab)

#create a node
node = bionet_node_new(hab, "test-node")
bionet_hab_add_node(hab, node)

hab_read()

#create a resource
resource = bionet_resource_new(node, 
                               BIONET_RESOURCE_DATA_TYPE_BINARY,
                               BIONET_RESOURCE_FLAVOR_PARAMETER,
                               "test-resource")


bionet_node_add_resource(node, resource)

#set the value of the resource
if (bionet_resource_set_binary(resource, 0, None)):
    print "Failed to set binary resource."


hab_report_new_node(node)

hab_read()

started = time()
fds = [hab_fd]
while (time() - started < 10):
    inputready,outputready,exceptready = select.select(fds,[],[], 1)
    hab_read()

sys.exit(0)
