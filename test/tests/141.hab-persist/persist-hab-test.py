#!/usr/bin/python

from hab import *
import logging
import optparse
import select
import time
import datetime
from ctypes import *
import sys, os

#set up logging
logger = logging.getLogger("Bionet Persist Test HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)

#parse the incoming command line
parser = optparse.OptionParser()
parser.add_option("-i", "--id", dest="id", default=None,
                  help="HAB-ID",
                  metavar="HAB-ID")

(options,args) = parser.parse_args()

bionet_log_use_default_handler(None)

hab = Hab("persist-test", options.id)
hp = HabPublisher(hab)

if hp.setPersistDirectory("."):
    logger.error("Failed to set the persistency directory to this local dir")
    exit(1)

node = Node(hp.hab, "test")
if (None == node):
    logger.error("Failed to create node, exiting")
    exit(1)
hp.hab.add(node)


bionet_types = { "binary" : BIONET_RESOURCE_DATA_TYPE_BINARY,
                 "uint8"  : BIONET_RESOURCE_DATA_TYPE_UINT8,
                 "int8"   : BIONET_RESOURCE_DATA_TYPE_INT8,
                 "uint16" : BIONET_RESOURCE_DATA_TYPE_UINT16,
                 "int16"  : BIONET_RESOURCE_DATA_TYPE_INT16,
                 "uint32" : BIONET_RESOURCE_DATA_TYPE_UINT32,
                 "int32"  : BIONET_RESOURCE_DATA_TYPE_INT32,
                 "float"  : BIONET_RESOURCE_DATA_TYPE_FLOAT,
                 "double" : BIONET_RESOURCE_DATA_TYPE_DOUBLE,
                 "string" : BIONET_RESOURCE_DATA_TYPE_STRING }

for k, v in sorted(bionet_types.iteritems()):
    resource = Resource(node, 
                        v, 
                        BIONET_RESOURCE_FLAVOR_SENSOR,
                        k);
    if (None == resource):
        logger.error("Failed to create resource, exiting")
        exit(1)
    
    hp.hab.node("test").add(resource)

    if (hp.persist(hp.hab.node("test").resource(k))):
        logger.warning("Not persisting resource %s" % resource.name())

    print ( "%s = %s %s %s" % (hp.hab.node("test").resource(k).name(),
                               hp.hab.node("test").resource(k).datatypeToString(),
                               hp.hab.node("test").resource(k).flavorToString(),
                               str(hp.hab.node("test").resource(k).datapoint(0))))

if (0 > hp.connect()):
    logger.warning("problem connecting HAB to Bionet, exiting")
    exit(1)

# report stuff
hp.reportNode(hp.hab.node("test"))
hp.reportDatapoints(hp.hab.node("test"))




i = 0
next_pub = 0
starttime = time.time()
while(time.time() - starttime < 30):
    (rr, wr, er) = select.select([hp.fd], [], [], 1.0)
    if (rr):
        hp.read()

    i += 1

    # for the first 5 seconds, do not publish new datapoints, wait for clients
    if (5 > time.time() - starttime):
        continue;

    if (next_pub <= time.time()):
        next_pub = time.time() + 1
    else: 
        continue

    for k, v in sorted(bionet_types.iteritems()):
        resource = hp.hab.node("test").resource(k)

        if (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_BINARY):
            resource.set(int(time.time() % 2))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_UINT8):
            resource.set(int(time.time() % 0xFF))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_INT8):
            resource.set(int(time.time() % 0x7F))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_UINT16):
            resource.set(int(time.time() % 0xFFFF))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_INT16):
            resource.set(int(time.time() % 0x7FFF))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_UINT32):
            resource.set(int(time.time()))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_INT32):
            resource.set(int(time.time()))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_FLOAT):
            resource.set(float(time.time() / 1000000.0))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_DOUBLE):
            resource.set(float(time.time() / 1000000.0))
        elif (resource.datatype() == BIONET_RESOURCE_DATA_TYPE_STRING):
            resource.set(str(time.time()))


        print ( "%s = %s %s %s" % (resource.name(),
                                   resource.datatypeToString(),
                                   resource.flavorToString(),
                                   str(resource.datapoint(0))))

    #report stuff
    hp.reportDatapoints(hp.hab.node("test"))

exit(0)
