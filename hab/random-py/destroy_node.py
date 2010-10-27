
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.

# This library is free software. You can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as 
# published by the Free Software Foundation, version 2.1 of the License.
# This library is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
# Lesser General Public License for more details. A copy of the GNU 
# Lesser General Public License v 2.1 can be found in the file named 
# “COPYING.LESSER”.  You should have received a copy of the GNU Lesser 
# General Public License along with this library; if not, write to the 
# Free Software Foundation, Inc., 
# 51 Franklin Street, Fifth Floor, 
# Boston, MA 02110-1301 USA.
 
# You may contact the Automation Group at:
# bionet@bioserve.colorado.edu
 
# Dr. Kevin Gifford
# University of Colorado
# Engineering Center, ECAE 1B08
# Boulder, CO 80309
 
# Because BioNet was developed at a university, we ask that you provide
# attribution to the BioNet authors in any redistribution, modification, 
# work, or article based on this library.
 
# You may contribute modifications or suggestions to the University of
# Colorado for the purpose of discussing and improving this software.
# Before your modifications are incorporated into the master version 
# distributed by the University of Colorado, we must have a contributor
# license agreement on file from each contributor. If you wish to supply
# the University with your modifications, please join our mailing list.
# Instructions can be found on our website at 
# http://bioserve.colorado.edu/developers-corner.



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

