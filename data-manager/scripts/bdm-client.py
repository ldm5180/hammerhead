#!/usr/bin/python
#
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#

import sys
import logging
import optparse
import re

# parse options 
parser = optparse.OptionParser()

parser.add_option("-o", "--output", dest="output", default="csv", 
                  help="Output format",
                  metavar="csv|matlab|chrono")
parser.add_option("-s", "--server", dest="server",
                  help="BDM server hostname.", 
                  default="localhost")
parser.add_option("-T", "--datapoint-start", dest="datapoint_start",
                  help="Datapoint Start Time", 
                  metavar="YYYY-MM-DD HH:MM:SS")
parser.add_option("-t", "--datapoint-end", dest="datapoint_end",
                  help="Datapoint End Time", 
                  metavar="YYYY-MM-DD HH:MM:SS")
parser.add_option("-r", "--resource", dest="resource", default="*.*.*:*",
                  help="Resource Name",
                  metavar="<HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>")
parser.add_option("-E", "--entry-start", dest="entry_start",
                  help="Entry Sequence Number Start",
                  default=-1)
parser.add_option("-e", "--entry-end", dest="entry_end",
                  help="Entry Sequence Number End",
                  default=-1)
parser.add_option("-p", "--port", dest="bdm_port",
                  help="BDM port number",
                  default=11002)

(options, args) = parser.parse_args()

#init logger
logger = logging.getLogger("BDM Client Output Formatter")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

if (options.output == "chrono"):
    li = []

import gobject
import time
from bdm_client import *
import calendar

# since bionet wants times in UTC...
def gmt_convert(loctime):
    if (time.daylight):
        return loctime - time.altzone
    else:
        return loctime - time.timezone

bdm_connect(options.server, options.bdm_port) 

# convert the start and end times into timevals
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

if (options.entry_start != None):
    entry_start = options.entry_start
else:
    entry_start = -1

if (options.entry_end != None):
    entry_end = options.entry_end;
else:
    entry_end = -1



# get the big HAB list
hab_list = bdm_get_resource_datapoints(options.resource, datapoint_start, datapoint_end, entry_start, entry_end)


for hi in range(hab_list.len):  # loop over all the HABs
    hab = bionet_get_hab(hab_list, hi); 
    
    # loop over all the nodes
    for ni in range(bionet_hab_get_num_nodes(hab)):  
        node = bionet_hab_get_node_by_index(hab, ni)

        for ri in range (bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, ri)
        

        # loop over all the datapoints
            if resource:
                for di in range (0, bionet_resource_get_num_datapoints(resource)): 
                    d = bionet_resource_get_datapoint_by_index(resource, di)
                    v = bionet_datapoint_get_value(d);

                    if (options.output == "csv"):
                        print bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + "." + bionet_node_get_id(node) + ":" + bionet_resource_get_id(resource) + "," + bionet_datapoint_timestamp_to_string(d) + "," + bionet_value_to_str(v) 
                    elif (options.output == "matlab"):
                        timematch = re.compile("((.*) (.*))\..*")
                        ts = bionet_datapoint_timestamp_to_string(d)
                        match = timematch.match(ts)
                        mlts = time.strftime("%Y:%m:%d\t%H:%M:%S", time.strptime(match.group(1), "%Y-%m-%d %H:%M:%S"))
                        print bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + "." + bionet_node_get_id(node) + ":" + bionet_resource_get_id(resource) + "\t" + mlts + "\t" + bionet_value_to_str(v) 
                    elif (options.output == "chrono"):
                        #sort me
                        output_string = bionet_datapoint_timestamp_to_string(d) + "," + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + "." + bionet_node_get_id(node) + ":" + bionet_resource_get_id(resource) + "," + bionet_value_to_str(v)          
                        li.append(output_string);
                    else:
                        print "Unknown format"
                        parser.usage()

bdm_disconnect() # done, disconnect

sorted_list = sorted(li)
for i in range(len(sorted_list)):
    print sorted_list[i]
