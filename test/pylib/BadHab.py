#!/usr/bin/python

import os
import sys
import bonjour
import select
import time
import threading
import socket



class BadHab:
    """A Hab class that lets you misbehave in a number of fun ways
    
    Among the possibilities are:
      - Publishing a service that times out on connect
    """

    regtype = "_bionet._tcp"

    __waitEvent = threading.Event()
    __resolveEvent = threading.Event()
    __port = 0
    __servicename = None
    __serviceRef = None

    # Failure modes
    __connectTimeout = 0
    __trickleSpeed = 0

    #
    # Private Callback functions
    #
    def __RegisterCallback(self,sdRef,flags,errorCode,name,regtype,domain,usrdata):
        #sys.stderr.write( "\nRegistered service %s (waiting for %s)\n" % (name, self.__servicename) )
        if name == self.__servicename:
            self.__waitEvent.set()

    def __ResolveCallback(self,sdRef,flags,interfaceIndex,
                        errorCode,fullname,hosttarget,
                        port,txtLen,txtRecord,usrdata):
        """ Called when bonjour resolves a service """

        if fullname.split('.')[0] == __servicename:
            sys.stderr.write( "\nResolved service %s\n" % (fullname) )
            self.__resolveEvent.set()

    def __BrowseCallback(self,sdRef,flags,interfaceIndex,
                 errorCode,serviceName,regtype,
                 replyDomain,lookFor):
        """ Called when bonjour finds a service """
        if serviceName == lookFor and flags & bonjour.kDNSServiceFlagsAdd:
            sys.stderr.write( "Found service; resolving...\n")

            self.__waitEvent.set()
            
            sdRef2 = bonjour.AllocateDNSServiceRef()
            ret = bonjour.pyDNSServiceResolve(sdRef2,
                                              0,
                                              0,
                                              serviceName,
                                              regtype,
                                              replyDomain,
                                              self.__ResolveCallback,
                                              None );

            self.__resolveEvent.clear()
            while not self.__resolveEvent.isSet():
                bonjour.DNSServiceProcessResult(sdRef2)

    def setFailModes(self,connectTimeout=False,trickleSpeed=0):
        """Set the ways this hab will misbehave.

          - connectTimeout: boolean -- If True, cause a connect 
          to the published service to timeout. Preceeds all other 
          options

          - trickle: int -- Set the bytes/sec rate that messages will
          sent and recvd at. (0 for unbounded)
          """
        
        self.__connectTimeout = connectTimeout
        self.__trickleSpeed = trickleSpeed



    def publish(self,name):
        txtRecordLength = 1
        txtRecord = ""
        interfaceIndex = 0
        flags = 0
        self.__servicename = name

        tcpSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        tcpSock.bind(("0.0.0.0",0))
        (addr,self.__port)=tcpSock.getsockname()
        #print "Addr: %s Port: %d" % (addr, self.port)


        #sys.stderr.write("Register service: %s\n" % (name,))
        try:
            ret = bonjour.pyDNSServiceRegister(self.__serviceRef,
                                      flags,
                                      interfaceIndex,
                                      name,
                                      self.regtype,
                                      None,
                                      None,
                                      socket.htons(self.__port),
                                      txtRecordLength,
                                      txtRecord,
                                      self.__RegisterCallback,
                                      self)
        except Exception, err:
            print err
            sys.exit(1)

        if ret != bonjour.kDNSServiceErr_NoError:
            print "register: ret = %lu; exiting" % ret
            sys.exit(1)


        # Get the socket and loop
        #self.__waitEvent.clear()
        fd = bonjour.DNSServiceRefSockFD(self.__serviceRef)
        while not self.__waitEvent.isSet():
            ret = select.select([fd],[],[])
            ret = bonjour.DNSServiceProcessResult(self.__serviceRef)

        print "Service %s registered at %s:%d" % (name,addr, self.__port)


        if self.__connectTimeout:
            print 'sudo iptables -A INPUT -p tcp --dport %d -j DROP' % (self.__port)
            os.system('sudo iptables -A INPUT -p tcp --dport %d -j DROP' 
                % (self.__port));
        tcpSock.listen(1)
        
    def unpublish(self):
        """Unpublish the service

        Remove the service from the mdns-sd published list and do any other
        cleanup needed"""

        if self.__serviceRef:
            if self.__connectTimeout:
                print 'sudo iptables -D INPUT -p tcp --dport %d -j DROP' % (self.__port)
                os.system('sudo iptables -D INPUT -p tcp --dport %d -j DROP' 
                    % (self.__port));


    def __init__(self):
        os.putenv("AVAHI_COMPAT_NOWARN", "1")
        self.__serviceRef = bonjour.AllocateDNSServiceRef()
        #self.publish()
        

    def __del__(self):
        print 'Destructor triggered'
        self.unpublish()
        if self.__serviceRef:
            bonjour.DNSServiceRefDeallocate(self.__serviceRef)

        self.__serviceRef = None


    def browse(self,name):
        sys.stderr.write("\nBrowsing for service '%s'\n" % (name))
        ret = bonjour.pyDNSServiceBrowse(  self.__serviceRef,
                                      0,
                                      0,
                                      self.regtype,
                                      None,
                                      self.__BrowseCallback,
                                      name)
        if ret != bonjour.kDNSServiceErr_NoError:
            print "browse: ret = %d; exiting" % ret
            sys.exit(1)

        #Block until service is found                     
        fd = bonjour.DNSServiceRefSockFD(self.__serviceRef)
        self.__waitEvent.clear()
        while not self.__waitEvent.isSet():
            ret = select.select([fd],[],[])
            ret = bonjour.DNSServiceProcessResult(self.__serviceRef)
        print "Found service: %s; resolving" % (__servicename)

        #Block until service is resolved
        while not self.__resolveEvent.isSet():
            time.sleep(.5)



#
# Main program
#

__hab = None

def __exit_handler(signum, frame):
    if (__hab != None ):
        __hab.unpublish()
    print '-- signal %d caught. Exiting --' % signum
    exit(0)

if __name__ == "__main__":
    import signal
    __hab = BadHab()

    host = socket.gethostname()
    name = 'bad-hab.' + host

    signal.signal(signal.SIGTERM, __exit_handler)
    signal.signal(signal.SIGINT, __exit_handler)

    __hab.setFailModes( connectTimeout = True )
    __hab.publish(name)

    time.sleep(30)

    __hab.unpublish()


