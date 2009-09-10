#!/usr/bin/python

import optparse
import twitter
from hab import *
import logging
import time
from ctypes import *
import calendar
import datetime
import twitterparse

def gmt_convert(loctime):
    if (time.daylight):
        return loctime - time.altzone
    else:
        return loctime - time.timezone

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


logger = logging.getLogger("Bionet Twitter HAB")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)


def pycb_set_resource(resource, value):
    logger.info("callback: sending Tweets is not yet implemented")


#initialize bionet security
if (options.security_dir != None):
    if (hab_init_security(options.security_dir, 1)):
        logger.warning("Failed to initialize security.")


users = set()

if (options.password != None):
    api = twitter.Api(username=options.user, password=options.password)
    if (options.tweeter != None):
        # password, but a tweeter requested, only subscribing to that one
        print "Ignoring follow list in favor of " + options.tweeter
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

#get a node and resource for this tweeter
for u in users:
    node_name = u.screen_name.replace('_', '-')
    node = bionet_node_new(hab, node_name.encode('utf-8'))

    name_resource = bionet_resource_new(node, 
                                        BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                        "name")
    bionet_node_add_resource(node, name_resource)

    tweet_resource = bionet_resource_new(node, 
                                         BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                         "tweet")
    bionet_node_add_resource(node, tweet_resource)

    tweet_source_resource = bionet_resource_new(node, 
                                         BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                         "tweet-source")
    bionet_node_add_resource(node, tweet_source_resource)

    screenname_resource = bionet_resource_new(node, 
                                              BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                              "screenname")
    bionet_node_add_resource(node, screenname_resource)

    user_description_resource = bionet_resource_new(node, 
                                                    BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                                    "user-description");
    bionet_node_add_resource(node, user_description_resource)

    profile_img_url_resource = bionet_resource_new(node, 
                                                   BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                                   "profile-img-url")
    bionet_node_add_resource(node, profile_img_url_resource)

    url_resource = bionet_resource_new(node, 
                                       BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                       "url")
    bionet_node_add_resource(node, url_resource)

    location_resource = bionet_resource_new(node, 
                                            BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                            "location")
    bionet_node_add_resource(node, location_resource)

    followers_count_resource = bionet_resource_new(node, 
                                                   BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                            "followers-count")
    bionet_node_add_resource(node, followers_count_resource)

    friends_count_resource = bionet_resource_new(node, 
                                            BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                            "friends-count")
    bionet_node_add_resource(node, friends_count_resource)

    protected_resource = bionet_resource_new(node, 
                                            BIONET_RESOURCE_DATA_TYPE_BINARY, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                            "protected")
    bionet_node_add_resource(node, protected_resource)

    statuses_count_resource = bionet_resource_new(node, 
                                            BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                            "tweet-count")
    bionet_node_add_resource(node, statuses_count_resource)

    utc_offset_resource = bionet_resource_new(node, 
                                            BIONET_RESOURCE_DATA_TYPE_INT32, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                            "utc-offset")
    bionet_node_add_resource(node, utc_offset_resource)

    tweet_id_resource = bionet_resource_new(node, 
                                            BIONET_RESOURCE_DATA_TYPE_UINT32, BIONET_RESOURCE_FLAVOR_SENSOR, 
                                            "tweet-id")
    bionet_node_add_resource(node, tweet_id_resource)


    #set the semi-constant user attributes
    if u.name:
        bionet_resource_set_str(name_resource, u.name.encode('utf-8'), None)
    if u.description:
        bionet_resource_set_str(user_description_resource, u.description.encode('utf-8'), None)
    if u.profile_image_url:
        bionet_resource_set_str(profile_img_url_resource, u.profile_image_url.encode('utf-8'), None)
    if u.screen_name:
        bionet_resource_set_str(screenname_resource, u.screen_name.encode('utf-8'), None)
    if u.url:
        bionet_resource_set_str(url_resource, u.url.encode('utf-8'), None)
    if u.location:
        bionet_resource_set_str(location_resource, u.location.encode('utf-8'), None)
    if u.status:
        tv = timeval()
        tv.tv_sec = gmt_convert(int(time.mktime(time.strptime(u.status.created_at, 
                                                              "%a %b %d %H:%M:%S +0000 %Y"))))
        tv.tv_usec = 0
        bionet_resource_set_str(tweet_resource, u.status.text.encode('utf-8'), tv)
        if (max_id < u.status.id):
            max_id = u.status.id
        
        bionet_resource_set_uint32(tweet_id_resource, u.status.id, tv)
        bionet_resource_set_str(tweet_source_resource, u.status.source.encode('utf-8'), tv)
    if u.followers_count:
        bionet_resource_set_uint32(followers_count_resource, u.followers_count, None)
    if u.friends_count:
        bionet_resource_set_uint32(friends_count_resource, u.friends_count, None)
    if u.protected:
        bionet_resource_set_binary(protected_resource, 1, None)
    else:
        bionet_resource_set_binary(protected_resource, 0, None)
    if u.statuses_count:
        bionet_resource_set_uint32(statuses_count_resource, u.statuses_count, None)
    if u.utc_offset:
        bionet_resource_set_int32(utc_offset_resource, u.utc_offset, None)
    else:
        bionet_resource_set_int32(utc_offset_resource, 0, None)

    bionet_hab_add_node(hab, node)

    hab_report_new_node(node)
    hab_report_datapoints(node)
    hab_read()


while(1):
    hab_read()
    date = datetime.datetime.now()
    time.sleep(int(options.frequency))
    updates = api.GetFriendsTimeline(since_id=max_id)
    for up in updates:
        if (max_id < up.id):
            max_id = up.id
        twitterparse.parse_status(hab, up)
        print up

