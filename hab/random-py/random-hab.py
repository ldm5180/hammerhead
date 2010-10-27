#!/usr/bin/python
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


import sys
import optparse
import logging
import time
import random
import datetime
from select import select 

parser = optparse.OptionParser()
parser.add_option("-i", "--id", dest="hab_id", default="python",
                  help="Use ID as the HAB-ID (defaults to hostname if omitted)",
                  metavar="ID")
parser.add_option("-n", "--min-nodes", dest="min_nodes", default=5,
                  help="If there are fewer than N nodes, make up some more.",
                  metavar="N")
parser.add_option("-x", "--max-delay", dest="max_delay",
                  help="After taking each action (adding or removing a Node, or updating a Resource), the random-hab sleeps up to this long (seconds)",
                  default=1,
                  metavar="X")
parser.add_option("-t", "--test", dest="test", default=None,
                  help="Output all data to a file formatted for testing against BDM.",
                  metavar="FILE")
parser.add_option("-l", "--loop", dest="loops", default=0,
                  help="Number of times to do node updates before quitting.")
parser.add_option("-s", "--security-dir", dest="security_dir", default=None,
                  help="Directory containing security certificates.")
parser.add_option("-d", "--die", dest="die", default=None,
                  help="Seconds to run before terminating.")

(options, args) = parser.parse_args()

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)


from hab import *
import add_node
import destroy_node
import update_node


def cb_set_resource(resource, value):
    print "callback: should set " + bionet_resource_get_local_name(resource) + " to " + bionet_value_to_str(value)


#initialize bionet security
if (options.security_dir != None):
    if (hab_init_security(options.security_dir, 1)):
        logger.warning("Failed to initialize security.")


#connect to bionet
hab = Hab("RANDOM", options.hab_id)
if (None == hab):
    logger.warning("problem connection to Bionet, exiting\n")
    exit(1)

habp = HabPublisher(hab)

habp.setResourceCallback = cb_set_resource;

habp.connect()

loops = 0

#test mode. open the output file and sleep to let subscribers catch up
if (options.test):
    f = open(options.test, "w", 0)
    time.sleep(3)
else:
    f = None;

if (f):
    cur = time.gmtime()
    time_str = "%(year)04d-%(month)02d-%(day)02d %(hour)02d:%(minute)02d:%(sec)02d.%(float)06d" % {'year' : cur.tm_year,
                                                                                                       'month' : cur.tm_mon,
                                                                                                       'day' : cur.tm_mday,
                                                                                                       'hour' : cur.tm_hour,
                                                                                                       'minute' : cur.tm_min,
                                                                                                       'sec' : cur.tm_sec,
                                                                                                       'float': datetime.datetime.now().microsecond }
    output_string = time_str + ",+H," + habp.hab.name() + "\n"
    f.write(output_string);


fd_list = []
if (habp.fd != -1):
    fd_list.append(habp.fd)

remaining = 3.0
elapsed = 0
start_time = time.time()

while ((int(options.loops) == 0) or (loops < int(options.loops))) and (None == options.die or time.time() - start_time < float(options.die)):
    # select on the fd list and wait for some seconds.
    curtime = time.time()
        
    (rr, wr, er) = select(fd_list, [], [], remaining)
    for fd in rr:
        if (fd == habp.fd):
            habp.read()
    elapsed += (time.time() - curtime)
    if (elapsed < remaining):
        remaining = float(options.max_delay) - elapsed
        continue
    else:
        while (habp.hab.numNodes() < options.min_nodes):
            add_node.Add(habp, f)

        while (habp.hab.numNodes() > 2 * options.min_nodes):
            destroy_node.Destroy(habp, f)

        rnd = random.randint(0,100)

        if (rnd < 10):
            destroy_node.Destroy(habp, f)
        elif (rnd < 20):
            add_node.Add(habp, f)
        else:
            update_node.Update(habp, f)
        
        loops = loops + 1
        remaining = float(options.max_delay)

if (f):
    cur = time.gmtime()
    time_str = "%(year)d-%(month)d-%(day)d %(hour)d:%(minute)d:%(sec)d.%(float)d" % {'year' : cur.tm_year,
                                                                                     'month' : cur.tm_mon,
                                                                                     'day' : cur.tm_mday,
                                                                                     'hour' : cur.tm_hour,
                                                                                     'minute' : cur.tm_min,
                                                                                     'sec' : cur.tm_sec,
                                                                                     'float': habp.hab.numNodes() }
    output_string = time_str + ",-H," + habp.hab.name() + "\n"
    f.write(output_string);

    i = 0;
    while (i < habp.hab.numNodes()):
        output_string = time_str + ",-N," + habp.hab.node(i).name() + "\n"
        f.write(output_string)
        i += 1

    f.close()
habp.hab.disconnect()
