
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


from hab import *
import random
import logging
import optparse 

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

def Destroy(habpublisher, options):
    node = habpublisher.hab.node(random.randint(0, habpublisher.hab.numNodes() - 1))
    if (node != None):

        logger.info("removing Node " + node.id())
        
        node = habpublisher.hab.remove(node.id())
        habpublisher.reportLostNode(node)

