#!/usr/bin/python
#
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


from bdm_client import *
from bdmplot_timespan import timeval_to_float
from bdmplot_callback_cacher import *


def datapoints_to_dict(timespan_vals, filter_string = "*.*.*:*", timespan = "last 6h", regexp = None, resources = None,
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
            for s in subscriptions:
                if ((bionet_resource_name_matches(name, s['resource name'])) and (timespan == s['timespan'])):
                    if (s['new'] > 0):
                        updated = True
                        dpcache['new'] = 0
                        results[name] = []
                        results[name] = dpcache['list']
                        s['last rendered'] = time.time()
                        s['new'] = 0
    return (updated, results)

if __name__ == "__main__":
    print datapoints_to_dict("last 7d", "eris-talker.*.*:limit")
