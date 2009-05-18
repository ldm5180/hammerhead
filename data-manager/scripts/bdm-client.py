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
parser.add_option("-b", "--begin", dest="starttime",
                  help="Start Time", 
                  metavar="YYYY-MM-DD HH:MM:SS")
parser.add_option("-e", "--end", dest="endtime",
                  help="End Time", 
                  metavar="YYYY-MM-DD HH:MM:SS")
parser.add_option("-r", "--resource", dest="resource", default="*.*.*:*",
                  help="Resource Name",
                  metavar="<HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>")

(options, args) = parser.parse_args()

if ((None == options.starttime) or (None == options.endtime)):
    parser.error("Must define start and end times.")

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

bdm_connect(options.server, BDM_PORT) 

# convert the start and end times into timevals
start = timeval()
start.tv_sec = gmt_convert(int(time.mktime(time.strptime(options.starttime, 
                                                         "%Y-%m-%d %H:%M:%S"))))
start.tv_usec = 0

end = timeval()
end.tv_sec = gmt_convert(int(time.mktime(time.strptime(options.endtime, 
                                                       "%Y-%m-%d %H:%M:%S"))))
end.tv_usec = 0

# get the big HAB list
hab_list = bdm_get_resource_datapoints(options.resource, start, end)


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
