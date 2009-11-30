#!/usr/bin/python
# bdmplot.py
# Main bdmplot generator.

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

from timespan import timespan_to_timevals, timeval_to_float
import timechooser

def bdmplot(**kwargs):
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
             "filter": "syshealth.*.*:15-min-load-average",
             "timespan": "last 6h",
             "regexp": None,
             "format": "png",
             "width": 7,
             "height": 5,
             "dpi": 60 }
    
    # Get args from the caller; these override the defaults but are 
    #         overriden by CGI.
    if kwargs != None and len(kwargs) > 0:
        for k,v in kwargs.iteritems():
            args[k] = v

    # Get values from CGI
    # CGI overrides any previous values.
    form = cgi.FieldStorage()
    if len(form) > 0:
        for k in form.keys():
            args[k] = form[k].value
    
    timespan_vals = timespan_to_timevals(args["timespan"])
    timespan_stamps = map(timeval_to_float, timespan_vals)
    timespan_stamps_now = map(lambda x : timeval_to_float(x, evaluateNow = True), timespan_vals)
    
    # Get the results
    from datapoints_to_dict import datapoints_to_dict
    results = datapoints_to_dict(timespan_vals, args["filter"], args["regexp"])

    # Split the dictionary into:
    #   - an array of legends based on the keys
    #   - a tuple of args for the lines
    legend = []
    plotargs = []
    if len(results) == 0:
        # If there are no results, matplotlib will barf on an empty set.  Add
        # two datapoints that are off-screen.
        plotargs.append([timespan_stamps_now[0]-100, timespan_stamps_now[0]-99])
        plotargs.append([0, 0])
    else:
        for k,v in results.iteritems():
            legend.append(k)
            vals_T = zip(*v)
            plotargs.append(vals_T[0])
            plotargs.append(vals_T[1])
            # FIXME
            # If the results have a bigger timestamp than our idea about "now", and we wanted to
            # plot until "now", expand the right plot boundary.
    
    # Plot the results
    import pylab
    f = pylab.figure(figsize=(int(args["width"]), int(args["height"])))
    pylab.step(*plotargs, **{'figure': f})

    # Set the x-axis interval and formatter appropriately
    ax = pylab.gca()
    ax.xaxis.set_major_formatter(timechooser.SmartDateFormatter(timespan_stamps_now))
    pylab.xlim( timespan_stamps_now )

    # Plot 11 major ticks (10 major segments)
    ax.xaxis.set_major_locator(matplotlib.ticker.LinearLocator(numticks=11))
    pylab.xticks(rotation = 33)

    # Render the plot.
    pylab.show()

    retval = { 'pylab' : pylab, 'args' : args }
    
    return retval



if __name__ == "__main__":
    retval = bdmplot()

    # If CGI, add MIME header
    if os.environ.has_key("GATEWAY_INTERFACE"):
        print "Content-Type: image/" + args["format"] + "\n"

    # Print the plot.
    retval['pylab'].savefig(os.sys.stdout, format=retval['args']["format"], dpi=retval['args']["dpi"])
