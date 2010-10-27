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

from bionet import *
from timespan import timeval_to_int

import time

sessions = {}
bionet_resources = {}

#callbacks
def cb_lost_hab(hab):
    for i in range(0, bionet_hab_num_nodes(hab)):
        node = bionet_hab_get_node_by_index(hab, i)
        for j in range(0, bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, j)
            pybionet_set_user_data(resource, None)
    print("lost hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab))


def cb_new_hab(hab):
    print("new hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab))


def cb_new_node(node):
    hab = bionet_node_get_hab(node)
	
    print("new node: " + bionet_node_get_name(node))
	
    if (bionet_node_get_num_resources(node)):
        print("    Resources:")
	    
        for i in range(bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, i)
            datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
		
            if (datapoint == None):
                print("        " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + bionet_resource_get_id(resource) + ": (no known value)")
            
            else:
                value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
                #%s %s %s = %s @ %s 
                print("        " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + bionet_resource_get_id(resource) + " = " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint))

    if (bionet_node_get_num_streams(node)):
        print("    Streams:")
    
        for i in range(bionet_node_get_num_streams(node)):
            stream = bionet_node_get_stream_by_index(node, i)
            print("        " + bionet_stream_get_id(stream) + " " + bionet_stream_get_type(stream) + " " + bionet_stream_direction_to_string(bionet_stream_get_direction(stream)))


def cb_lost_node(node):
    hab = bionet_node_get_hab(node)
    for j in range(0, bionet_node_get_num_resources(node)):
        resource = bionet_node_get_resource_by_index(node, j)
        pybionet_set_user_data(resource, None)
    print("lost node: " + bionet_node_get_name(node))


def cb_datapoint(datapoint):
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);
    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);
    
    value_str = bionet_value_to_str(value);
    #"%s.%s.%s:%s = %s %s %s @ %s"    
    #print(bionet_resource_get_name(resource) + " = " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint))

    now = time.time()
    
    resource_name = bionet_resource_get_name(resource)
    dp = (timeval_to_int(bionet_datapoint_get_timestamp(datapoint)), value_str)

    removal = []

    for session_id, session in sessions.iteritems():
        found = False
        #print "Checking session", session_id
        for r in session['resource']:
            if (1 == bionet_resource_name_matches(resource_name, r)):
                #print resource_name, " matches ", r
                for name in session['bionet-resources']:
                    if (name == resource_name):
                        u = bionet_resources[name]
                        if (None == u) or ('datapoints' not in u) or ('sessions' not in u): # no user data is set yet
                            u = { 'datapoints' : [ dp ], 'sessions' : { session_id : [ dp ] } }
                            bionet_resources[name] = u
                            #print "Added datapoint to new user data"
                        else: # user data is set, just append to it
                            u['datapoints'].append(dp)
                            if session_id in u['sessions']:
                                u['sessions'][session_id].append(dp)
                            else:
                                u['sessions'][session_id] = [ dp ]
                            #print "Added datapoint to existing user data"
                        
                        found = True
                        
                if (False == found):
                    session['bionet-resources'].append(resource_name)
                    u = { 'datapoints' : [ dp ], 'sessions' : { session_id : [ dp ] } }
                    bionet_resources[resource_name] = u
                    #print "Added datapoint to new user data of new resource"

        if (now > (session['last requested'] + 600)):
            # this session hasn't been requested in more than 10 minutes. remove it
            removal.append(session_id)
                
    for session_id in removal:
        #print "removed subscription ", sessions[session_id]['resource']
        del sessions[session_id]
        # TODO: unsubscribe when bionet_unsubscribe() is implemented

    #print "Datapoint added"
