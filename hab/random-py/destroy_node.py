

from hab import *
import random

def Destroy(hab):
    node = bionet_hab_get_node_by_index(hab, random.randint(0, bionet_hab_get_num_nodes(hab)))
    if (node != None):

        print("removing Node " + bionet_node_get_id(node))

        bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node))
        hab_report_lost_node(bionet_node_get_id(node))
        bionet_node_free(node)

