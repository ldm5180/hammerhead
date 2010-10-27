#!/usr/bin/python

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

import optparse
import twitter
from hab import *
import logging
import time
from ctypes import *
import calendar
import datetime
import twitterparse
import select

def gmt_convert(loctime):
    if (time.daylight):
        return loctime - time.altzone
    else:
        return loctime - time.timezone

#parse the incoming command line
parser = optparse.OptionParser()
parser.add_option("-t", "--tweeter", dest="tweeter", default=None,
                  help="Twitter to Follow",
                  metavar="USERNAME")
parser.add_option("-u", "--user", dest="user", default="BioServeSpace",
                  help="Twitter User ID",
                  metavar="USERNAME")
parser.add_option("-p", "--password", dest="password", default=None,
                  help="Twitter Password",
                  metavar="PASSWORD")
parser.add_option("-f", "--frequency", default=300, dest="frequency",
                  help="Refresh frequency",
                  metavar="Seconds")
parser.add_option("-s", "--security-dir", dest="security_dir", default=None,
                  help="Directory containing security certificates.")

(options,args) = parser.parse_args()


#set up logging
logger = logging.getLogger("Bionet Twitter HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)


#some day this will tweet!
def pycb_set_resource(resource, value):
    logger.info("callback: sending Tweets is not yet implemented")


#initialize bionet security
if (options.security_dir != None):
    if (hab_init_security(options.security_dir, 1)):
        logger.warning("Failed to initialize security.")


users = set()

#if a username and password were supplied then use them
if (options.password != None):
    api = twitter.Api(username=options.user, password=options.password)
    if (options.tweeter != None):
        # password, but a tweeter requested, only subscribing to that one
        logger.info( "Ignoring follow list in favor of " + options.tweeter)
        users.add(options.tweeter)
    else:
        # password and default follow list requested, getting friend list
        friends = api.GetFriends()
        for f in friends:
            users.add(f)
else:
    api = twitter.Api()
    if (options.tweeter == None):
        # no password and no tweeter, lets follow @NASA
        users.add(api.GetUser("NASA"))
    else:
        # no password, but a tweeter requested, follow that only
        users.add(api.GetUser(options.tweeter))
      
  
me = api.GetUser(options.user)

#connect to bionet
hab = bionet_hab_new("Twitter", me.screen_name.encode('utf-8'))
pyhab_register_callback_set_resource(pycb_set_resource)
bionet_fd = hab_connect(hab)
if (0 > bionet_fd):
    logger.warning("problem connection to Bionet, exiting\n")
    exit(1)

time.sleep(2)
hab_read()

max_id = 0

#get a node and resources for this tweeter
for u in users:
    max_id = twitterparse.process_user(hab, u, max_id)


sleep_time = float(options.frequency)

while(1):
    hab_read()

    pre = datetime.datetime.now()
    (rr, wr, er) = select.select([bionet_fd], [], [], sleep_time)
    expired = datetime.datetime.now() - pre
    
    if (rr):
        hab_read()
    
    if (expired.seconds >= sleep_time):
        sleep_time = float(options.frequency)
        updates = api.GetFriendsTimeline(since_id=max_id)
        for up in updates:
            if (max_id < up.id):
                max_id = up.id
                twitterparse.parse_status(hab, up)

    else:
        sleep_time = sleep_time - expired.seconds
                
