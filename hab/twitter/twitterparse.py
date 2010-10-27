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
# "COPYING.LESSER".  You should have received a copy of the GNU Lesser 
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

import twitter
from hab import *
import time
from ctypes import *
import calendar
import datetime

def gmt_convert(loctime):
    if (time.daylight):
        return loctime - time.altzone
    else:
        return loctime - time.timezone

def parse_status(hab, update):
    node_name = update.user.screen_name.replace('_', '-')
    n = bionet_hab_get_node_by_id(hab, node_name.encode('utf-8'))

    #time of the tweet
    tv = timeval()
    tv.tv_sec = gmt_convert(int(time.mktime(time.strptime(update.created_at, 
                                                          "%a %b %d %H:%M:%S +0000 %Y"))))
    tv.tv_usec = 0

    #text of the tweet
    r = bionet_node_get_resource_by_id(n, "tweet")
    bionet_resource_set_str(r, update.text.encode('utf-8'), tv)
    
    #tweet id
    r = bionet_node_get_resource_by_id(n, "tweet-id")
    bionet_resource_set_uint32(r, update.id, tv)

    #tweet source
    r = bionet_node_get_resource_by_id(n, "tweet-source")
    bionet_resource_set_str(r, update.source.encode('utf-8'), tv)

    #followers count
    r = bionet_node_get_resource_by_id(n, "followers-count")
    bionet_resource_set_uint32(r, update.user.followers_count, None)

    #tweet count
    r = bionet_node_get_resource_by_id(n, "tweet-count")
    bionet_resource_set_uint32(r, update.user.statuses_count, None)
    
    hab_report_datapoints(n)


def process_user(hab, u, max_id):
    node_name = u.screen_name.replace('_', '-')
    node = bionet_node_new(hab, node_name.encode('utf-8'))

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "name")
    bionet_node_add_resource(node, r)
    if u.name:
        bionet_resource_set_str(r, u.name.encode('utf-8'), None)

    tweet_source_resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "tweet-source")
    bionet_node_add_resource(node, tweet_source_resource)

    tweet_id_resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, "tweet-id")
    bionet_node_add_resource(node, tweet_id_resource)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "tweet")
    bionet_node_add_resource(node, r)
    if u.status:
        tv = timeval()
        tv.tv_sec = gmt_convert(int(time.mktime(time.strptime(u.status.created_at, 
                                                              "%a %b %d %H:%M:%S +0000 %Y"))))
        tv.tv_usec = 0
        bionet_resource_set_str(r, u.status.text.encode('utf-8'), tv)
        if (max_id < u.status.id):
            max_id = u.status.id
        
        bionet_resource_set_uint32(tweet_id_resource, u.status.id, tv)
        bionet_resource_set_str(tweet_source_resource, u.status.source.encode('utf-8'), tv)


    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "screenname")
    bionet_node_add_resource(node, r)
    if u.screen_name:
        bionet_resource_set_str(r, u.screen_name.encode('utf-8'), None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "user-description");
    bionet_node_add_resource(node, r)
    if u.description:
        bionet_resource_set_str(r, u.description.encode('utf-8'), None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "profile-img-url")
    bionet_node_add_resource(node, r)
    if u.profile_image_url:
        bionet_resource_set_str(r, u.profile_image_url.encode('utf-8'), None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "url")
    bionet_node_add_resource(node, r)
    if u.url:
        bionet_resource_set_str(r, u.url.encode('utf-8'), None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, "location")
    bionet_node_add_resource(node, r)
    if u.location:
        bionet_resource_set_str(r, u.location.encode('utf-8'), None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, "followers-count")
    bionet_node_add_resource(node, r)
    if u.followers_count:
        bionet_resource_set_uint32(r, u.followers_count, None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, "friends-count")
    bionet_node_add_resource(node, r)
    if u.friends_count:
        bionet_resource_set_uint32(r, u.friends_count, None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_BINARY, BIONET_RESOURCE_FLAVOR_SENSOR, "protected")
    bionet_node_add_resource(node, r)
    if u.protected:
        bionet_resource_set_binary(r, 1, None)
    else:
        bionet_resource_set_binary(r, 0, None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, "tweet-count")
    bionet_node_add_resource(node, r)
    if u.statuses_count:
        bionet_resource_set_uint32(r, u.statuses_count, None)

    r = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_INT32, BIONET_RESOURCE_FLAVOR_SENSOR, "utc-offset")
    bionet_node_add_resource(node, r)
    if u.utc_offset:
        bionet_resource_set_int32(r, u.utc_offset, None)
    else:
        bionet_resource_set_int32(r, 0, None)


    bionet_hab_add_node(hab, node)

    hab_report_new_node(node)
    hab_report_datapoints(node)
    hab_read()

    return max_id;
