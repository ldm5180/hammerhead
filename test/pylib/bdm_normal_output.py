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
    bdmlog.write("lost bdm: " + bionet_bdm_get_id(bdm) + '\n')


def cb_new_bdm(bdm):
    bdmlog.write("new bdm: " + bionet_bdm_get_id(bdm) + '\n')

def cb_lost_hab(hab):
    bdmlog.write("lost hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + '\n')


def cb_new_hab(hab):
    bdmlog.write("new hab: " + bionet_hab_get_type(hab) + "." + bionet_hab_get_id(hab) + '\n')


def cb_new_node(node):
    hab = bionet_node_get_hab(node)
	
    bdmlog.write("new node: " + bionet_node_get_name(node) + '\n')
	
def cb_lost_node(node):
    hab = bionet_node_get_hab(node);
    bdmlog.write("lost node: " + bionet_node_get_name(node) + '\n')


def cb_datapoint(datapoint):
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);
    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);
    
    #bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)) + "," +
    #bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)) + "," + 
    value_str = bionet_value_to_str(value);
    bdmlog.write(
            bionet_datapoint_timestamp_to_string(datapoint) + "," +
            bionet_resource_get_name(resource) + "," + 
            value_str + "\n") 
    
