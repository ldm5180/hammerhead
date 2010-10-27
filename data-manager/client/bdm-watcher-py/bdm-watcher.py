#!/usr/bin/python

from twisted.internet import reactor
from bdm_client import *
import logging
import optparse
from select import select


# initialize the Python logger
logger = logging.getLogger("BDM Watcher")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

# parse options 
parser = optparse.OptionParser()

parser.add_option("-o", "--output-file", dest="file", default=None, 
                  help="File to output in BDM chrono format.")
parser.add_option("-T", "--datapoint-start", dest="datapoint_start",
                  help="Datapoint Start Time", 
                  metavar="YYYY-MM-DD HH:MM:SS")
parser.add_option("-t", "--datapoint-end", dest="datapoint_end",
                  help="Datapoint End Time", 
                  metavar="YYYY-MM-DD HH:MM:SS")
parser.add_option("-f", "--filter", dest="filter",
                  help="Datapoint filter.", 
                  default="*.*.*:*")
(options, args) = parser.parse_args()

if None == options.file:
    f = None
else:
    f = open(options.file, "w", 0)


# New HAB callback is called each time a new HAB matching the subscription joins the Bionet network
def cb_new_hab(hab, event):
    global f
    logger.info("new hab: %s" % hab.name())
    if f:
        output_str = event.timestampToString() + ",+H," + hab.name() + "\n"
        f.write(output_str);

# Lost HAB callback is called each time a HAB matching the subscription disconnects from the Bionet network
def cb_lost_hab(hab, event):
    logger.info("lost hab: %s" % hab.name())
    if f:
        output_str = event.timestampToString() + ",-H," + hab.name() + "\n"
        f.write(output_str);

# New Node callback is called each time a HAB reports a new node matching the subscription
def cb_new_node(node, event):
    logger.info("  new node: %s" % node.name())
    if f:
        output_str = event.timestampToString() + ",+N," + node.name() + "\n"
        f.write(output_str);

    logger.info("    resources:");
    for i in range(node.numResources()):
        resource = node.resource(i)
        logger.info("        %(datatype)s %(flavor)s %(id)s" % {"datatype" : resource.datatypeToString(),
                                                                "flavor" : resource.flavorToString(),
                                                                "id" : resource.id() } )
        if f:
            output_str = event.timestampToString() + ",+R," + resource.name() + "," + resource.datatypeToString() + " " + resource.flavorToString() + "\n"
            f.write(output_str);
            for j in range(resource.numDatapoints()):
                output_str = event.timestampToString() + ",+D," + resource.name() + "," + resource.datatypeToString() + " " + resource.flavorToString() + " " + str(resource.datapoint(j)) + "\n"
                f.write(output_str);

# Lost Node callback is called each time a HAB reports a lost node matching the subscription
def cb_lost_node(node, event):
    logger.info("  lost node: %s" % node.name())
    if f:
        output_str = event.timestampToString() + ",-N," + node.name() + "\n"
        f.write(output_str);

# Datapoint callback is called each time a HAB publishes a datapoint matching the subscription
def cb_datapoint(datapoint, event):
    resource = datapoint.resource()
    value = datapoint.value()
    logger.info("%(name)s = %(datapoint)s", {"name" : resource.name(),
                                             "datapoint" : str(datapoint) } )

    if f:
        output_str = event.timestampToString() + ",+D," + datapoint.resource().name() + "," + datapoint.resource().datatypeToString() + " " + datapoint.resource().flavorToString() + " " + str(datapoint) + "\n"
        f.write(output_str);


# set the default Bionet log handler
bionet_log_use_default_handler(None)

# connect to Bionet as a client
bdm_sub = BdmSubscriber()

# register all the callbacks desired
bdm_sub.newHabCallback = cb_new_hab
bdm_sub.lostHabCallback = cb_lost_hab
bdm_sub.newNodeCallback = cb_new_node
bdm_sub.lostNodeCallback = cb_lost_node
bdm_sub.datapointCallback = cb_datapoint

# subscribe to HABs, Nodes, and Datapoints. In this example, all HABs and Nodes 
# are subscribed, but only datapoints from the Sample HAB.

(hab_type, hab_id, local_res_name) = options.filter.rsplit('.')
(node_id, resource_id) = local_res_name.rsplit(':')

bdm_sub.subscribeToHab(hab_type + "." + hab_id)
bdm_sub.subscribeToNode(hab_type + "." + hab_id + "." + node_id)
bdm_sub.subscribeToDatapoints(options.filter, options.datapoint_start, options.datapoint_end)

fd_list = []
if (bdm_sub.fd != -1):
    fd_list.append(bdm_sub.fd)

while(1):
    # select on the fd list
    (rr, wr, er) = select(fd_list, [], [], None)
    for fd in rr:
        if (fd == bdm_sub.fd):
            read = bdm_sub.read() # causes the callbacks to get called
