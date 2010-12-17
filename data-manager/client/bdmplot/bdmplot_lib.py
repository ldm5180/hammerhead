#!/usr/bin/python
# bdmplot.py
# Main bdmplot generator.
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

import cgi
import cgitb
cgitb.enable()

# Import matplotlib in such a way that it doesn't leak all over stdout and it
# doesn't wedge when it can't write all over HOME
import os
os.environ [ 'HOME' ] = '/tmp/'
import matplotlib
matplotlib.use('Agg')
import matplotlib.dates as mdates

from bdmplot_timespan import timespan_to_timevals, timeval_to_float
import timechooser

def bdmplot(kwargs, bionet_resources):
    """
    Plots a time series of Bionet Data Manager datapoints on a Matplotlib
    plot, renders that to png, and prints the png to stdout.  If called from 
    CGI, the MIME header "Content-Type: image/png" is prepended.
    
    If this script is placed in a cgi-bin directory of a webserver, users can
    get plots of datapoints contained in your BDM.  Users can pass arguments
    using HTTP GET.  For example, a URL to render all the syshealth-hab's
    uptime datapoints over the last day might look like:

    http://yourwebserver.com/cgi-bin/bdmplot.py?filter=syshealth.*.*:uptime&timespan=last 1d
    """
    # Supply default values
    args = { "label": "plot",
             "resource": ["syshealth.*.*:15-min-load-average"],
             "timespan": ["last 6h"],
             "regexp": None,
             "format": "png",
             "width": [7],
             "height": [5],
             "dpi": 60,
             "bionet-resources" : {},
             "resource name" : [],
             "axis" : [],
             }

    # Get args from the caller; these override the defaults but are 
    #         overriden by CGI.
    if kwargs != None and len(kwargs) > 0:
        for k,v in kwargs.iteritems():
            args[k] = v


    #build the file name from the set of resources in the plot and the timespan
    fname = "/tmp/"
    for x in args['resource']:
        if (x.count("/")):
            (bdm_ids, resource_name) = x.split("/")
        else:
            resource_name = x

        # while we are at it, add the resource name to the list
        args['resource name'].append(resource_name)

        fname += resource_name + "." 
    fname += args['timespan'][0]


    # Get values from CGI
    # CGI overrides any previous values.
    form = cgi.FieldStorage()
    if len(form) > 0:
        for k in form.keys():
            args[k] = form[k].value
    
    timespan_vals = timespan_to_timevals(args["timespan"][0])
    timespan_stamps = map(timeval_to_float, timespan_vals)
    timespan_stamps_now = map(lambda x : timeval_to_float(x, evaluateNow = True), timespan_vals)
    
    # Get the results
    from bdmplot_datapoints_to_dict import datapoints_to_dict

    import pylab
    f = pylab.figure(figsize=(int(args["width"][0]), int(args["height"][0])))
    title = ""
    
    legend = []
    resource_index = 0
    for resource in args['resource name']:
        plotargs = []

        (updated, results) = datapoints_to_dict(timespan_vals, resource, args['timespan'], args["regexp"], bionet_resources)
        
        # if it hasn't been updated then just use the cached image
        if (updated == False):
            return (fname, args['format'])

        # Split the dictionary into:
        #   - an array of legends based on the keys
        #   - a tuple of args for the lines
        plottypes = ["r-", "g-"]
        if len(results) == 0:
            # If there are no results, matplotlib will barf on an empty set.  Add
            # two datapoints that are off-screen.
            plotargs.append([timespan_stamps_now[0]-100, timespan_stamps_now[0]-99])
            plotargs.append([0, 0])
        else:
            for k,v in results.iteritems():
                legend.append(k)
                vals_T= zip(*v)
                plotargs.append(vals_T[0])
                plotargs.append(vals_T[1])
                plotargs.append(plottypes[resource_index])
                # FIXME
                # If the results have a bigger timestamp than our idea about "now", and we wanted to
                # plot until "now", expand the right plot boundary.
            resource_index += 1
        
        ax = pylab.gca()

#TODO : add axis labels
#TODO : add name at the top of the image
#TODO : add axis labels to the file name if available.

        if (resource_index == 2) and (len(args["axis"])):
            ax = ax.twinx()

        # Plot the results
        pylab.step(*plotargs, **{'figure': f})


    # Set the x-axis interval and formatter appropriately
    ax.xaxis.set_major_formatter(timechooser.SmartDateFormatter(timespan_stamps_now))
    pylab.xlim( timespan_stamps_now )

    # Plot 11 major ticks (10 major segments)
    ax.xaxis.set_major_locator(matplotlib.ticker.LinearLocator(numticks=11))
    pylab.xticks(rotation = 33)

    # Render the plot.
    pylab.show()

    pylab.savefig(fname, format=args["format"], dpi=args["dpi"])
    pylab.close()
    
    return (fname, args['format'])



if __name__ == "__main__":
    retval = bdmplot()

    # If CGI, add MIME header
    if os.environ.has_key("GATEWAY_INTERFACE"):
        print "Content-Type: image/" + args["format"] + "\n"

    # Print the plot.
    retval['pylab'].savefig(os.sys.stdout, format=retval['args']["format"], dpi=retval['args']["dpi"])
