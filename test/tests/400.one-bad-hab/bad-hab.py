#!/usr/bin/python

import os
import sys
import bonjour
import select
import time
import threading
from socket import *
from signal import *

#
# Globals 
#
waitEvent = threading.Event()
resolveEvent = threading.Event()


#
# Callback functions
#

# Register 
def RegisterCallback(sdRef,flags,errorCode,name,regtype,domain,userdata):
    if name == servicename:
        sys.stderr.write( "Service registered\n")
        waitEvent.set()

# Resolve
def ResolveCallback(sdRef,flags,interfaceIndex,
                    errorCode,fullname,hosttarget,
                    port,txtLen,txtRecord,userdata):
    if fullname.split('.')[0] == servicename:
        sys.stderr.write( "\nResolved service %s: host,port=%s,%d\n" % (fullname,host,port) )
        resolveEvent.set()

# Browse
def BrowseCallback(sdRef,flags,interfaceIndex,
             errorCode,serviceName,regtype,
             replyDomain,userdata):
    if serviceName == servicename and flags & bonjour.kDNSServiceFlagsAdd:
        sys.stderr.write( "Found service; resolving...\n")

        waitEvent.set()
        
        sdRef2 = bonjour.AllocateDNSServiceRef()
        ret = bonjour.pyDNSServiceResolve(sdRef2,
                                          0,
                                          0,
                                          serviceName,
                                          regtype,
                                          replyDomain,
                                          ResolveCallback,
                                          None );

        resolveEvent.clear()
        while not resolveEvent.isSet():
            bonjour.DNSServiceProcessResult(sdRef2)


#
# Main program
#


flags = 0
interfaceIndex = 0
host = gethostname()
servicename = 'bad-hab.' + host
regtype = "_bionet._tcp"
domain = "local."
port = 0
txtRecordLength = 1
txtRecord = ""
userdata = None

os.putenv("AVAHI_COMPAT_NOWARN", "1")


def exit_handler(signum, frame):
    if (port > 0):
        os.system('sudo iptables -D INPUT -p tcp --dport %d -j DROP' % (port));
    print '-- signal %d caught. Exiting --' % signum
    exit(0)


    

tcpSock = socket(AF_INET, SOCK_STREAM)
tcpSock.bind(("0.0.0.0",0))
(addr,port)=tcpSock.getsockname()
#print "Addr: %s Port: %d" % (addr, port)

signal(SIGTERM, exit_handler)
signal(SIGINT, exit_handler)


#sys.stderr.write("Register service: %s\n" % (servicename,))
serviceRef = bonjour.AllocateDNSServiceRef()
try:
    ret = bonjour.pyDNSServiceRegister(serviceRef, 
                              flags,                  
                              interfaceIndex,                  
                              servicename,        
                              regtype,            
                              None,           
                              None,           
                              htons(port),               
                              txtRecordLength,                  
                              txtRecord,               
                              RegisterCallback,   
                              userdata)
except Exception as err:
    print err
    sys.exit(1)

if ret != bonjour.kDNSServiceErr_NoError:
    print "register: ret = %lu; exiting" % ret
    sys.exit(1)

print "Service %s registered at %s:%d" % (servicename,addr, port)

# Get the socket and loop
fd = bonjour.DNSServiceRefSockFD(serviceRef)
while not waitEvent.isSet():
    ret = select.select([fd],[],[])
    ret = bonjour.DNSServiceProcessResult(serviceRef)


#sys.stderr.write("\nBrowsing for service\n")
#ret = bonjour.pyDNSServiceBrowse(  serviceRef,  
#                              0,                   
#                              0,                   
#                              regtype,             
#                              'local.',            
#                              BrowseCallback,      
#                              None)                
#if ret != bonjour.kDNSServiceErr_NoError:
#    print "browse: ret = %d; exiting" % ret
#    sys.exit(1)

# Block until service is found                     
#fd = bonjour.DNSServiceRefSockFD(serviceRef)
#waitEvent.clear()
#while not waitEvent.isSet():
#    ret = select.select([fd],[],[])
#    ret = bonjour.DNSServiceProcessResult(serviceRef)
#print "Found service: %s; resolving" % (servicename)

# Block until service is resolved
#while not resolveEvent.isSet():
#    time.sleep(.5)


#stopat = time.clock() + 30;

#while(time.clock() < stopat):
#    data = tcpSock.recv(1024)

os.system('sudo iptables -A INPUT -p tcp --dport %d -j DROP' % (port));
tcpSock.listen(1)
time.sleep(60)
os.system('sudo iptables -D INPUT -p tcp --dport %d -j DROP' % (port));


# Cleanup
bonjour.DNSServiceRefDeallocate(serviceRef)


