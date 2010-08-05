#!/usr/bin/python
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


import sys
import logging
from bdm_client import *

bdmlog=sys.stdout

class BDMNormalOutput:

    def __init__(self,log=None):
        if log:
            global bdmlog
            bdmlog = open(log, "w")


    def handle_callbacks(self):
        pybdm_register_callback_new_bdm(cb_new_bdm, None)
        pybdm_register_callback_lost_bdm(cb_lost_bdm, None);

        pybdm_register_callback_new_hab(cb_new_hab, None)
        pybdm_register_callback_lost_hab(cb_lost_hab, None);

        pybdm_register_callback_new_node(cb_new_node, None);
        pybdm_register_callback_lost_node(cb_lost_node, None);

        pybdm_register_callback_datapoint(cb_datapoint, None);


#callbacks
def cb_lost_bdm(bdm):
    pass



def cb_new_bdm(bdm):
    pass

def cb_lost_hab(hab, event):
    bdmlog.write("%s,-H,%s\n" % (bionet_event_get_timestamp_as_str(event), bionet_hab_get_name(hab)))


def cb_new_hab(hab, event):
    bdmlog.write("%s,+H,%s\n" % (bionet_event_get_timestamp_as_str(event), bionet_hab_get_name(hab)))


def cb_new_node(node, event):
    bdmlog.write("%s,+N,%s\n" % (bionet_event_get_timestamp_as_str(event), bionet_node_get_name(node)))

    for i in range(bionet_node_num_resources(node)):
	resource = bionet_node_get_resource_by_index(node, i)
	res_flavor = bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
	res_type = bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource))
	bdmlog.write("%s,+N,%s,%s %s\n" % (bionet_event_get_timestamp_as_str(event, timestamp_str, 64), bionet_resource_get_name(resource), res_type, res_flavor))

	
def cb_lost_node(node, event):
    bdmlog.write("%s,-N,%s\n" % (bionet_event_get_timestamp_as_str(event), bionet_node_get_name(node)))


def cb_datapoint(datapoint, event):
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);

    res_flavor = bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
    res_type = bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource))
    value_str = bionet_value_to_str(value);

    bdmlog.write("%s,+D,%s,%s %s %s @ %s\n" % (bionet_event_get_timestamp_as_str(event), bionet_resource_get_name(resource), res_type, res_flavor, value_str, bionet_datapoint_timestamp_to_string(datapoint)))
    
