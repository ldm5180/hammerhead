from bdm_client import *
from timespan import *

def prune_datapoints(subscriptions, bionet_resources):
    removal_list = []
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

        # see if there are no subscriptions which want this resource at all
        if (oldest_timeval == None):
            #print "removing entire bionet resource: ", name
            # don't remove items in a dict while iterating the dict
            removal_list.append(name)

        # get rid of datapoints older than the oldest timeval
        for dp in dpcache['list']:
            if (dp[0] < oldest_timeval):
                dpcache['list'].remove(dp)
                dpcache['new'] += 1
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
