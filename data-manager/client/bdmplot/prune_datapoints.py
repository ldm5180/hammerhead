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

from bdm_client import *
from bdmplot_timespan import *
from bdmplot_callback_cacher import *

def prune_datapoints(subscriptions, bionet_resources):
    removal_list = []
    for name, dpcache in bionet_resources.iteritems():
        oldest_timeval = None

        # go through the subscriptions and get the subscription requesting the oldest datapoints for this resource
        for sub in subscriptions:
            if (bionet_resource_name_matches(name, sub['resource name'])):
                timespan_vals = timespan_to_timevals(sub['timespan'][0])
                tvfloat = timeval_to_float(timespan_vals[0])
                if (oldest_timeval == None):
                    oldest_timeval = tvfloat
                elif (tvfloat < oldest_timeval):
                    oldest_timeval = tvfloat

        # see if there are no subscriptions which want this resource at all
        if (oldest_timeval == None):
            #print "removing entire bionet resource: ", name
            # don't remove items in a dict while iterating the dict
            removal_list.append(name)

        # get rid of datapoints older than the oldest timeval
        for dp in dpcache['list']:
            if (dp[0] < oldest_timeval):
                dpcache['list'].remove(dp)
                for s in subscriptions:
                    if (bionet_resource_name_matches(name, s['resource name'])):
                        s['new'] += 1

                #print "Removed dp: ", dp
            else:
                break

    # done iterating the dict, so remove the ones that need to be removed now
    for name in removal_list:
        bionet_resources.pop(name)


def prune_subscriptions(subscriptions):
    current = time.time()
    removed = False
    for sub in subscriptions:
        if ((current - 60) >= sub['last requested']):
            #print "removing old subscription: %(name)s at %(span)s" % { 'name' : sub['filter'], 'span' : sub['timespan'] }
            #TODO: unsubscribe from BDM
            subscriptions.remove(sub)
            removed = True

    return removed
