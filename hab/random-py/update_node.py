
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


from hab import *
import random
import set_random_resource_value
import logging

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

def Update(hab, f):
    node = bionet_hab_get_node_by_index(hab, random.randint(0, bionet_hab_get_num_nodes(hab)))

    if (node != None):
        logger.info("updating Resources on Node " + bionet_node_get_id(node))
        if (0 == bionet_node_get_num_resources(node)):
            logger.info("    no Resources, skipping")
        for i in range(bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, i);
            

            #resources are only updated 50% of the time
            if ((random.randint(0,1)) == 0):
                logger.info("    " + bionet_resource_get_id(resource) + " " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " = " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " = *** skipped")
            else:
                set_random_resource_value.Set(resource)

                datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
                value = bionet_datapoint_get_value(datapoint)
                hab = bionet_node_get_hab(node)
                logger.info("    " + bionet_resource_get_id(resource) + " " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " = " + bionet_value_to_str(bionet_datapoint_get_value(datapoint)))
                if (f):
                    output_string = bionet_datapoint_timestamp_to_string(datapoint) + "," + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + "." + bionet_node_get_id(node) + ":" + bionet_resource_get_id(resource) + "," + bionet_value_to_str(value) + "\n"
                    f.write(output_string)


            if (hab_report_datapoints(node)):
                logger.warning("PROBLEM UPDATING!!!\n");

