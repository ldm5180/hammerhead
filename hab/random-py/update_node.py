
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

def Update(hab, options):
    node = bionet_hab_get_node_by_index(hab, random.randint(0, bionet_hab_get_num_nodes(hab)))
    if (node != None):
        logger.info("updating Resources on Node " + bionet_node_get_id(node))
        if (0 == bionet_node_get_num_resources(node)):
            logger.info("    no Resources, skipping")
        for i in range(bionet_node_get_num_resources(node)):
            resource = bionet_node_get_resource_by_index(node, i);
            

            #resources are only updated 50% of the time
            if ((random.randint(0,1)) == 0):
                logger.info("    " + bionet_resource_get_id(resource) + " " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " = " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " = *** skipped\n")
            else:
                set_random_resource_value.Set(resource)

                datapoint = bionet_resource_get_datapoint_by_index(resource, 0)
                logger.info("    " + bionet_resource_get_id(resource) + " " + bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + " " + bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + " = " + bionet_value_to_str(bionet_datapoint_get_value(datapoint)))

            if (hab_report_datapoints(node)):
                logger.warning("PROBLEM UPDATING!!!\n");

