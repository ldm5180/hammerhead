#!/usr/bin/python

from twisted.internet import interfaces
from bionet import *

class Client():
    """twisted_bionet.Client eats a Bionet Client and implements the
    twisted.internet.interfaces.IReadDescriptor interface."""

    def __init__(self):
        self.bionet_fd = bionet_connect()
        if self.bionet_fd < 0:
            # FIXME: retry later?
            print "problem connecting to Bionet, exiting"
            reactor.stop()
        print "Twisted Bionet Client initialized."


    def doRead(self):
        bionet_read()

    def fileno(self):
        return self.bionet_fd

    def connectionLost(self, reason):
        print "connection to bionet is lost:", reason

    def logPrefix(self):
        return 'BionetClient'
