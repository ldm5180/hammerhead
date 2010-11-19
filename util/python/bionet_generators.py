#!/usr/bin/python
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#

def nodeGenerator(hab):
    nodelist = range(hab.numNodes())
    for i in nodelist:
        yield hab.node(i)

def resourceGenerator(node):
    reslist = range(node.numResources())
    for i in reslist:
        yield node.resource(i)

def datapointGenerator(resource):
    dplist = range(resource.numDatapoints())
    for i in dplist:
        yield resource.datapoint(i)

