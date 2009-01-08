
from hab import *
import node_ids
import resource_ids
import random
import logging
import set_random_resource_value
import optparse

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

def add_resource(node, options):
    while(1):
        resource_id = random.choice(resource_ids.resources)
        if (bionet_node_get_resource_by_id(node, resource_id) == None):
            break

    flavor = random.randint(0, 2)
    data_type = random.randint(0, 2)

    resource = bionet_resource_new(node, data_type, flavor, resource_id)
    if (resource == None):
        logger.warning("Error creating Resource")
        return
    r = bionet_node_add_resource(node, resource)
    if (r != 0):
        logger.warning("Error adding Resource")

    if ((random.randint(0,1)) == 0):
        logger.info(resource_id + " " + bionet_resource_data_type_to_string(data_type) + " " +  bionet_resource_flavor_to_string(flavor) + " = " + "(starts with no value)")
    else:
        set_random_resource_value.Set(resource)
        datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
        value = bionet_datapoint_get_value(datapoint)
        if (value):
            logger.info(resource_id + " " + bionet_resource_data_type_to_string(data_type) + " " + bionet_resource_flavor_to_string(flavor) + " = " + bionet_value_to_str(value))
        else:
            logger.info(resource_id + " " + bionet_resource_data_type_to_string(data_type) + " " + bionet_resource_flavor_to_string(flavor) + " = " + "No Value")


def Add(random_hab, options):
    while(1):
        node_id = random.choice(node_ids.names)
        if (bionet_hab_get_node_by_id(random_hab, node_id) == None): 
            break;

    logger.info("new Node " + node_id);

    node = bionet_node_new(random_hab, node_id)
    
    #add 0-29 resources
    num_resources = random.randint(0,30)
    for i in range(num_resources):
        add_resource(node, options)
        
    if (bionet_hab_add_node(random_hab, node) != 0):
        logger.warning("HAB failed to add Node")
	return;
    
    hab_report_new_node(node)

