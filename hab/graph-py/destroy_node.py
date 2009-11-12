
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


from hab import *
import random
import logging
import optparse 

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

def Destroy(hab, options):
    node = bionet_hab_get_node_by_index(hab, random.randint(0, bionet_hab_get_num_nodes(hab)))
    if (node != None):

        logger.info("removing Node " + bionet_node_get_id(node))
        
        bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node))
        hab_report_lost_node(bionet_node_get_id(node))
        bionet_node_free(node)

