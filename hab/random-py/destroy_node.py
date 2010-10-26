
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


from hab import *
import random
import logging
import optparse 
import time
import datetime

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

loop_index = 0

def Destroy(habpublisher, f):
    global loop_index
    node = habpublisher.hab.node(random.randint(0, habpublisher.hab.numNodes() - 1))
    if (node != None):

        logger.info("removing Node " + node.id())

        if (f):
            loop_index += 1
            cur = time.gmtime()
            time_str = "%(year)04d-%(month)02d-%(day)02d %(hour)02d:%(minute)02d:%(sec)02d.%(float)06d" % {'year' : cur.tm_year,
                                                                                             'month' : cur.tm_mon,
                                                                                             'day' : cur.tm_mday,
                                                                                             'hour' : cur.tm_hour,
                                                                                             'minute' : cur.tm_min,
                                                                                             'sec' : cur.tm_sec,
                                                                                             'float' : datetime.datetime.now().microsecond}
            output_string = time_str + ",-N," + node.name() + "\n"
            f.write(output_string);

        
        node = habpublisher.hab.remove(node.id())
        habpublisher.reportLostNode(node)

