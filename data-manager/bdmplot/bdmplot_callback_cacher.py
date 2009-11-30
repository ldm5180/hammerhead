from bdm_client import *
from timespan import *

subscriptions = []
bionet_resources = {}

#callbacks
def cb_lost_bdm(bdm, user_data):
    None


def cb_new_bdm(bdm, user_data):
    None


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
    print(bionet_resource_get_name(resource) + " = " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " " + value_str + " @ " + bionet_datapoint_timestamp_to_string(datapoint))
    
    resource_name = bionet_resource_get_name(resource)
    found = False
    dp = (bionet_datapoint_get_timestamp(datapoint), float(value_str))
    for sub in subscriptions:
        for r in sub['filter']:
            if (bionet_resource_name_matches(resource_name, r)):
                for name in sub['bionet-resources']:
                    if (name == resource_name):
                        u = bionet_resources[name]
                        if (None == u) or ('datapoints' not in u): # no user data is set yet
                            u = { 'datapoints' : [ dp ] }
                            bionet_resources[name] = u
                            print "Added datapoint to new user data"
                        else: # user data is set, just append to it
                            u['datapoints'].append(dp)
                            print "Added datapoint to existing user data"
                        
                        found = True
                        
                if (False == found):
                    sub['bionet-resources'].append(resource_name)
                    u = { 'datapoints' : [ dp ] }
                    bionet_resources[resource_name] = u
                    print "Added datapoint to new user data of new resource"
                     
    print "Datapoint added"
