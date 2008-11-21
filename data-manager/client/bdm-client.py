#!/usr/bin/python

import sys


# initialization
nag_hostname = "localhost"
USAGE = sys.argv[0] + " [--server SERVER] ResourceNamePattern StartTime EndTime"
argv_offset = 1;


#process argv
assert (4 <= len(sys.argv)), USAGE

if ((sys.argv[1] == "-s") or (sys.argv[1] == "--server") and (len(sys.argv) > 1)):
    nag_hostname = sys.argv[2]
    argv_offset = 3;

assert ((4 < len(sys.argv)) and (1 != argv_offset)), USAGE

resource_name_pattern = sys.argv[argv_offset]
start_time = sys.argv[argv_offset+1]
end_time = sys.argv[argv_offset+2]    


import gobject_wrapper
import time
from bdm_client import *
import calendar

# since bionet wants times in UTC...
def gmt_convert(loctime):
    if (time.daylight):
        return loctime - time.altzone
    else:
        return loctime - time.timezone

bdm_connect(nag_hostname, BDM_PORT)  # connect to the nag

# convert the start and end times into timevals
start = timeval()
start.tv_sec = gmt_convert(int(time.mktime(time.strptime(start_time, 
                                                         "%Y-%m-%d %H:%M:%S"))))
start.tv_usec = 0

end = timeval()
end.tv_sec = gmt_convert(int(time.mktime(time.strptime(end_time, 
                                                       "%Y-%m-%d %H:%M:%S"))))
end.tv_usec = 0

# get the big HAB list
hab_list = bdm_get_resource_datapoints(resource_name_pattern, start, end)


for hi in range(0, hab_list.len):  # loop over all the HABs
    hab = bdm_get_hab(hab_list, hi); 
    print hab.type + "." + hab.id  # print the hab type.id
    
    # loop over all the nodes
    for ni in range(0, bdm_hab_get_num_nodes(hab)):  
        node = bdm_hab_get_node_by_index(hab, ni);
        print "    " + node.id  # print the node id
        
        # loop over all the resources
        for ri in range(0, bdm_node_get_num_resources(node)):  
            resource = bdm_node_get_resource_by_index(node, ri);
            print "        " + bdm_resource_data_type_to_string(resource.data_type), bdm_resource_flavor_to_string(resource.flavor), resource.id

            # loop over all the datapoints
            for di in range (0, bdm_resource_get_num_datapoints(resource)): 
                d = bdm_resource_get_datapoint_by_index(resource, di)
                print "            " + bdm_datapoint_value_to_string(d),'@', bdm_datapoint_timestamp_to_string(d)


bdm_disconnect() # done, disconnect

