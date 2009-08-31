
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

