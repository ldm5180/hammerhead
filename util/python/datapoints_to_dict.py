#!/usr/bin/python
#


from bdm_client import *
from timespan import timeval_to_float

def datapoints_to_dict(timespan_vals, filter_string = "*.*.*:*", regexp = None,
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
    # Connect to the BDM.
    if bdm_fd == None:
        try:
            bdm_fd = bdm_connect(bdm_hostname, bdm_port)
        except:
            raise SystemError("Can't connect to BDM")
        if bdm_fd < 0:
            raise SystemError("Can't connect to BDM")
    
    # Get the HAB list, prefiltered.
    hab_list=bdm_get_resource_datapoints(filter_string, timespan_vals[0], timespan_vals[1], -1, -1)

    # If there's a regular expression, compile it.
    re_compiled = None
    if regexp != None:
        import re
        re_compiled = re.compile(regexp);
    
    # Stuff the result dictionary.
    results = {}
    for hi in range(bdm_get_hab_list_len(hab_list)):
        hab = bdm_get_hab_by_index(hab_list, hi);
        
        for ni in range(bionet_hab_get_num_nodes(hab)):
            node = bionet_hab_get_node_by_index(hab, ni)
            nodeid = bionet_node_get_id(node)
            
            for ri in range(bionet_node_get_num_resources(node)):
                res = bionet_node_get_resource_by_index(node, ri)
                resname = bionet_resource_get_name(res)
                # If there's a regex, and it doesn't match a resource, skip.
                if (re_compiled != None) and (re_compiled.search(resname) == None):
                    continue
                # Otherwise, stuff the resource and its datapoints in the result set.
                results[resname] = []
                for di in range(0, bionet_resource_get_num_datapoints(res)):
                    dp = bionet_resource_get_datapoint_by_index(res, di)
                    v  = bionet_datapoint_get_value(dp)
                    vs = bionet_value_to_str(v)
                    tv = timeval_to_float(bionet_datapoint_get_timestamp(dp))
                    results[resname].append([ tv, float(vs) ])
    return results

if __name__ == "__main__":
    print datapoints_to_dict("last 7d", "eris-talker.*.*:limit")
