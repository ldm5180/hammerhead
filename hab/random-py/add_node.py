
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


from hab import *
import node_ids
import resource_ids
import random
import logging
import set_random_resource_value
import optparse
import time
import calendar
import re

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

def add_resource(node, f):
    while(1):
        resource_id = random.choice(resource_ids.resources)
        if (node.resource(resource_id) == None):
            break

    flavor = random.randint(0, 2)
    data_type = random.randint(0, 2)

    resource = Resource(node, data_type, flavor, resource_id)
    if (resource == None):
        logger.warning("Error creating Resource")
        return
    r = node.add(resource)
    if (r != 0):
        logger.warning("Error adding Resource")

    if ((random.randint(0,1)) == 0):
        logger.info("    " + resource_id + " " + resource.datatypeToString() + " " +  resource.flavorToString() + " = " + "(starts with no value)")
    else:
        set_random_resource_value.Set(resource)
        datapoint = resource.datapoint(0)
        value = datapoint.value()
        hab = node.hab()

        if (value):
            logger.info("    " + resource_id + " " + resource.datatypeToString() + " " +  resource.flavorToString() + " = " + str(value))
            if (f):
                output_string = datapoint.timestampToString() + "," + hab.type() + "." + hab.id() + "." + node.id() + ":" + resource.id() + "," + str(value) + "\n"
                f.write(output_string)

        else:
            logger.info("    " + resource_id + " " + resource.datatypeToString() + " " +  resource.flavorToString() + " = " + "No Value")


def Add(habpublisher, f):
    while(1):
        node_id = random.choice(node_ids.names)
        if (habpublisher.hab.node(node_id) == None): 
            break;

    logger.info("new Node " + node_id);

    node = Node(habpublisher.hab, node_id)
    
    #add 0-29 resources
    num_resources = random.randint(0,30)
    for i in range(num_resources):
        add_resource(node, f)
        
    if (habpublisher.hab.add(node) != 0):
        logger.warning("HAB failed to add Node")
	return;
    
    habpublisher.reportNode(habpublisher.hab.node(node_id))

