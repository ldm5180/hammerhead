from bdm_client import *
from timespan import *

def prune_datapoints(subscriptions, bionet_resources):
    for name, dpcache in bionet_resources.iteritems():
        oldest_timeval = None

        # go through the subscriptions and get the subscription requesting the oldest datapoints for this resource
        for sub in subscriptions:
            if (bionet_resource_name_matches(name, sub['filter'])):
                timespan_vals = timespan_to_timevals(sub['timespan'][0])
                tvfloat = timeval_to_float(timespan_vals[0])
                if (oldest_timeval == None):
                    oldest_timeval = tvfloat
                elif (tvfloat < oldest_timeval):
                    oldest_timeval = tvfloat

        # get rid of datapoints older than the oldest timeval
        for dp in dpcache['list']:
            if (dp[0] < oldest_timeval):
                dpcache['list'].remove(dp)
                dpcache['new'] += 1
                print "Removed dp: ", dp
            else:
                break

def prune_subscriptions(subscriptions):
    None
