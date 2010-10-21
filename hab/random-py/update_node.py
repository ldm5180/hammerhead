
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


from hab import *
import random
import set_random_resource_value
import logging

logger = logging.getLogger("Bionet Random HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)

def Update(habpublisher, f):
    node = habpublisher.hab.node(random.randint(0, habpublisher.hab.numNodes() - 1))

    if (node != None):
        logger.info("updating Resources on Node " + node.id())
        if (0 == node.numResources()):
            logger.info("    no Resources, skipping")
        for i in range(node.numResources()):
            resource = node.resource(i)
            

            #resources are only updated 50% of the time
            if ((random.randint(0,1)) == 0):
                logger.info("    " + resource.id() + " " + resource.datatypeToString() + " = " + resource.flavorToString() + " = *** skipped")
            else:
                set_random_resource_value.Set(resource)

                datapoint = resource.datapoint(0)
                value = datapoint.value()
                hab = node.hab()
                logger.info("    " + resource.id() + " " + resource.datatypeToString() + " = " + resource.flavorToString() + " = " + str(value))
                if (f):
                    output_string = datapoint.timestampToString() + "," + resource.name() + "," + str(value) + "\n"
                    f.write(output_string)


            if (habpublisher.reportDatapoints(node)):
                logger.warning("PROBLEM UPDATING!!!\n");

