#!/usr/bin/python
#


from bdm_client import *
from timespan import timeval_to_float

def datapoints_to_dict(timespan_vals, filter_string = "*.*.*:*", regexp = None, resources = None,
    bdm_hostname = "localhost", bdm_port = 11002, bdm_fd = None):
    """
    Gets a bunch of datapoints from the Bionet Data Manager (BDM) and converts
    them to a dictionary of lists.
   
    timespan is a string representing a timespan over which to collect data,
    and can be specified in multiple formats.  The most explicit timespan
    is a defined start time and end time: 
        "2008-01-01 12:00:00 to 2010-03-04 13:45:00"
    The latter endpoint can also be "now":
        "2008-01-01 12:00:00 to now"
    Times can also be specified as "since":
        "since 2008-10-31" (equivalent to "2008-10-31 to now")
    Or "last":
        "last 24d"
    
    filter_string is a normal Bionet subscription string, with possible
    wildcards (like "eris-talker.*.*:CGBA5-UPL").  This is passed to BDM to
    pre-filter results.  If not supplied, it defaults to match all resources.
    
    regex is a compiled regular expression.  The results returned from BDM are
    further filtered through regex (if supplied) before being added to the 
    result set.

    Note: filter_string is a performance enhancement: all the filtering that
    can be performed in filter_string can just as easily be done in regex.
    However, the regex is applied after BDM executes the database query, so if
    you do not supply filter_string, the BDM query may take longer.
    """

    # If there's a regular expression, compile it.
    re_compiled = None
    if regexp != None:
        import re
        re_compiled = re.compile(regexp);
    
    updated = False

    if (len(resources) == 0):
        updated = True

    # Stuff the result dictionary.
    results = {}
    for name, dpcache in resources.iteritems():
        if (re_compiled != None):
            m = re_compiled.match(name)
            if (not m):
                #print name, " does not match regex ", regexp
                continue
        if (bionet_resource_name_matches(name, filter_string)):
            if (dpcache['new'] > 0):
                updated = True
                dpcache['new'] = 0
                results[name] = []
                results[name] = dpcache['list']
    return (updated, results)

if __name__ == "__main__":
    print datapoints_to_dict("last 7d", "eris-talker.*.*:limit")
