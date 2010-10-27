#!/usr/bin/python
#

# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.

# This library is free software. You can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as 
# published by the Free Software Foundation, version 2.1 of the License.
# This library is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
# Lesser General Public License for more details. A copy of the GNU 
# Lesser General Public License v 2.1 can be found in the file named 
# “COPYING.LESSER”.  You should have received a copy of the GNU Lesser 
# General Public License along with this library; if not, write to the 
# Free Software Foundation, Inc., 
# 51 Franklin Street, Fifth Floor, 
# Boston, MA 02110-1301 USA.
 
# You may contact the Automation Group at:
# bionet@bioserve.colorado.edu
 
# Dr. Kevin Gifford
# University of Colorado
# Engineering Center, ECAE 1B08
# Boulder, CO 80309
 
# Because BioNet was developed at a university, we ask that you provide
# attribution to the BioNet authors in any redistribution, modification, 
# work, or article based on this library.
 
# You may contribute modifications or suggestions to the University of
# Colorado for the purpose of discussing and improving this software.
# Before your modifications are incorporated into the master version 
# distributed by the University of Colorado, we must have a contributor
# license agreement on file from each contributor. If you wish to supply
# the University with your modifications, please join our mailing list.
# Instructions can be found on our website at 
# http://bioserve.colorado.edu/developers-corner.


import sys
import logging
import optparse
import re
import time

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
                  help="BDM port number", type="int",
                  default=11002)
parser.add_option("-f", "--frequency", dest="frequency", 
                  help="Query the BDM every N seconds",
                  type="int", default=0)

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

import time
from bdm_client import *
import calendar

# since bionet wants times in UTC...
def gmt_convert(loctime):
    if (time.daylight):
        return loctime - time.altzone
    else:
        return loctime - time.timezone

bdm_fd = bdm_connect(options.server, options.bdm_port) 
if (bdm_fd < 0):
    logger.error("Failed to connect to Bionet Data Manager.")
    sys.exit(1)

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


while 1:
    need_print = 0
    li = []

    # get the big HAB list
    hab_list = bdm_get_resource_datapoints(options.resource, datapoint_start, datapoint_end, entry_start, entry_end)

    for hi in range(bdm_get_hab_list_len(hab_list)):  # loop over all the HABs
        hab = bdm_get_hab_by_index(hab_list, hi); 

	# Log the hab's events
	if (options.output == "csv" or options.output == "chrono"):
            for ei in range(bionet_hab_get_num_events(hab)):
                event = bionet_hab_get_event_by_index(hab, ei)
                if bionet_event_get_type(event) == BIONET_EVENT_LOST:
                    typestr = '-H'
                else:
                    typestr = '+H'
                output_string = "%s,%s,%s" % (bionet_event_get_timestamp_as_str(event), typestr, bionet_hab_get_name(hab))
                if (options.output == "csv"):
                    print output_string
                else:
                    li.append(output_string);
                    need_print = 1
                    
        # loop over all the nodes
        for ni in range(bionet_hab_get_num_nodes(hab)):  
            node = bionet_hab_get_node_by_index(hab, ni)

            if (options.output == "csv" or options.output == "chrono"):
                for ei in range(bionet_node_get_num_events(node)):
                    event = bionet_node_get_event_by_index(node, ei)
                    if bionet_event_get_type(event) == BIONET_EVENT_LOST:
                        typestr = '-N'
                    else:
                        typestr = '+N'

                    output_string = "%s,%s,%s" % (bionet_event_get_timestamp_as_str(event), typestr, bionet_node_get_name(node))
                    if (options.output == "csv"):
                        print output_string
                    else:
                        li.append(output_string);
                        need_print = 1

                    if bionet_event_get_type(event) == BIONET_EVENT_PUBLISHED:
                        for ri in range (bionet_node_get_num_resources(node)):
                            resource = bionet_node_get_resource_by_index(node, ri)
                            res_flavor = bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
                            res_type = bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource))

                            if resource:
                                output_string = "%s,+R,%s,%s %s" % (bionet_event_get_timestamp_as_str(event), bionet_resource_get_name(resource), res_type, res_flavor)
                                if (options.output == "csv"):
                                    print output_string
                                else:
                                    li.append(output_string);
                                    need_print = 1
            
            for ri in range (bionet_node_get_num_resources(node)):
                resource = bionet_node_get_resource_by_index(node, ri)

                res_flavor = bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
                res_type = bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource))

                # loop over all the datapoints
                if resource:
                    for di in range (0, bionet_resource_get_num_datapoints(resource)): 
                        d = bionet_resource_get_datapoint_by_index(resource, di)
                        v = bionet_datapoint_get_value(d);

                        for ei in range(bionet_datapoint_get_num_events(d)):
                            event = bionet_datapoint_get_event_by_index(d, ei)
                            
                            if (options.output == "csv"):
                                print "%s,+D,%s,%s %s %s @ %s" % (bionet_event_get_timestamp_as_str(event), bionet_resource_get_name(resource), res_type, res_flavor,  bionet_value_to_str(v), bionet_datapoint_timestamp_to_string(d))
                            elif (options.output == "matlab"):
                                timematch = re.compile("((.*) (.*))\..*")
                                ts = bionet_datapoint_timestamp_to_string(d)
                                match = timematch.match(ts)
                                mlts = time.strftime("%Y:%m:%d\t%H:%M:%S", time.strptime(match.group(1), "%Y-%m-%d %H:%M:%S"))
                                print bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + "." + bionet_node_get_id(node) + ":" + bionet_resource_get_id(resource) + "\t" + mlts + "\t" + bionet_value_to_str(v) 
                            elif (options.output == "chrono"):
                                #sort me
                                output_string =  "%s,+D,%s,%s %s %s @ %s" % (bionet_event_get_timestamp_as_str(event), bionet_resource_get_name(resource), res_type, res_flavor,  bionet_value_to_str(v), bionet_datapoint_timestamp_to_string(d))
                                li.append(output_string);
                                need_print = 1
                            else:
                                print "Unknown format"
                                parser.usage()
                            
    if (need_print):
        sorted_list = sorted(li)
        for i in range(len(sorted_list)):
            print sorted_list[i]

    if (options.frequency <= 0):
        break;
    else:
        if (bdm_get_hab_list_last_entry_seq(hab_list)):
            entry_start = bdm_get_hab_list_last_entry_seq(hab_list) + 1
        bdm_hab_list_free(hab_list);
        time.sleep(options.frequency)
        
bdm_disconnect() # done, disconnect

