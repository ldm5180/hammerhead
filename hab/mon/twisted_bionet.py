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
# “COPYING.LESSER”.  You should have received a copy of the GNU Lesser 
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

from twisted.internet import interfaces
from hab import *


#
# FIXME: these should be in the swig somewhere
#

bionet_flavor = {
    'sensor':    BIONET_RESOURCE_FLAVOR_SENSOR,
    'actuator':  BIONET_RESOURCE_FLAVOR_ACTUATOR,
    'parameter': BIONET_RESOURCE_FLAVOR_PARAMETER
}

bionet_datatype = {
    'binary': BIONET_RESOURCE_DATA_TYPE_BINARY,
    'uint8':  BIONET_RESOURCE_DATA_TYPE_UINT8,
    'int8':   BIONET_RESOURCE_DATA_TYPE_INT8,
    'uint16': BIONET_RESOURCE_DATA_TYPE_UINT16,
    'int16':  BIONET_RESOURCE_DATA_TYPE_INT16,
    'uint32': BIONET_RESOURCE_DATA_TYPE_UINT32,
    'int32':  BIONET_RESOURCE_DATA_TYPE_INT32,
    'float':  BIONET_RESOURCE_DATA_TYPE_FLOAT,
    'double': BIONET_RESOURCE_DATA_TYPE_DOUBLE,
    'string': BIONET_RESOURCE_DATA_TYPE_STRING
}




class HAB():
    """twisted_bionet.HAB eats a bionet_hab_t and implements the
    twisted.internet.interfaces.IReadDescriptor interface."""

    def __init__(self, bionet_hab):
        self.bionet_hab = bionet_hab
        self.bionet_fd = hab_connect(self.bionet_hab)
        if self.bionet_fd < 0:
            # FIXME: retry later?
            print "problem connection to Bionet, exiting"
            reactor.stop()
        print "Twisted HAB initialized (%s)" % bionet_hab_get_name(self.bionet_hab)


    def doRead(self):
        hab_read()

    def fileno(self):
        return self.bionet_fd

    def connectionLost(self, reason):
        print "connection to bionet is lost:", reason

    def logPrefix(self):
        return 'HAB'

