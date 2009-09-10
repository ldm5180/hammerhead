#!/usr/bin/python

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

