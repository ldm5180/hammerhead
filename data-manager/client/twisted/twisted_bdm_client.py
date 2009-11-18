#!/usr/bin/python

from twisted.internet import interfaces
from bdm_client import *

class BdmClient():
    """twisted_bionet.Client eats a Bionet Client and implements the
    twisted.internet.interfaces.IReadDescriptor interface."""

    def __init__(self):
        self.bdm_fd = bdm_connect()
        if self.bdm_fd < 0:
            # FIXME: retry later?
            print "problem connecting to BDM, exiting"
            reactor.stop()
        print "Twisted BDM Client initialized."


    def doRead(self):
        bdm_read()

    def fileno(self):
        return self.bdm_fd

    def connectionLost(self, reason):
        print "connection to bionet is lost:", reason

    def logPrefix(self):
        return 'BdmClient'
