
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
# "COPYING.LESSER".  You should have received a copy of the GNU Lesser 
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
        logger.info("    " + resource_id + " " + bionet_resource_data_type_to_string(data_type) + " " +  bionet_resource_flavor_to_string(flavor) + " = " + "(starts with no value)")
    else:
        set_random_resource_value.Set(resource)
        datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
        value = bionet_datapoint_get_value(datapoint)
        hab = bionet_node_get_hab(node)

        if (value):
            logger.info("    " + resource_id + " " + bionet_resource_data_type_to_string(data_type) + " " + bionet_resource_flavor_to_string(flavor) + " = " + bionet_value_to_str(value))
            if (f):
                output_string = bionet_datapoint_timestamp_to_string(datapoint) + "," + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + "." + bionet_node_get_id(node) + ":" + bionet_resource_get_id(resource) + "," + bionet_value_to_str(value) + "\n"
                f.write(output_string)

        else:
            logger.info("    " + resource_id + " " + bionet_resource_data_type_to_string(data_type) + " " + bionet_resource_flavor_to_string(flavor) + " = " + "No Value")


def Add(random_hab, f):
    while(1):
        node_id = random.choice(node_ids.names)
        if (bionet_hab_get_node_by_id(random_hab, node_id) == None): 
            break;

    logger.info("new Node " + node_id);

    node = bionet_node_new(random_hab, node_id)
    
    #add 0-29 resources
    num_resources = random.randint(0,30)
    for i in range(num_resources):
        add_resource(node, f)
        
    if (bionet_hab_add_node(random_hab, node) != 0):
        logger.warning("HAB failed to add Node")
	return;
    
    hab_report_new_node(node)

