#!/usr/bin/python
# timechooser.py
# Chooses a reasonable format for plotting against a time axis.

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

from datetime import datetime, timedelta

class timeprinter:
    """
    When called, formats 'value' (which should be a POSIX timestamp as a
    float) according to strftime() using the supplied fstring.
    """
    def __init__(self, fstring = '%Y'):
        self.fstring = fstring
    def __call__(self, value, pos):
        return datetime.utcfromtimestamp(value).strftime(self.fstring)

# Define some useful printers
YearsPrinter     = timeprinter('%Y')
DayOfYearPrinter = timeprinter('%j/%H:%M')
MinutesPrinter   = timeprinter("%H:%M")
SecondsPrinter   = timeprinter('%H:%M:%S')

def SmartPrinter(delta):
    """ 
    Convert a time delta to the smartest timeprinter for that span.

    delta should either be: a duration in (float) seconds, a start
    time and end time in (float) seconds, or a timedelta.
    """

    # Convert arg into a timedelta
    if hasattr(delta, 'days'):
        td = delta
    elif len(delta) == 1:
        td = timedelta(seconds = delta)
    else:
        td = timedelta(seconds = (delta[1]-delta[0]))
    
    # Return a printer based on how big the timedelta is.
    if td.days >= 365:
        return YearsPrinter
    if td.days >= 1:
        return DayOfYearPrinter
    if td.seconds > 900:
        return MinutesPrinter
    return SecondsPrinter

try:
    from matplotlib.ticker import FuncFormatter
    def SmartDateFormatter(delta):
        return FuncFormatter(SmartPrinter(delta))
except:
    print "Timechooser doesn't have SmartFormatter"

def demo():
    # Plot a sine wave with our SmartFormatter on the x-axis.
    from matplotlib.ticker import MultipleLocator, FuncFormatter
    from pylab import arange, sin, exp, pi, subplot, plot, xticks, show 
    majorFormatter = FuncFormatter(DayOfYearPrinter)
    
    t = arange(0.0, 100.0, 1)
    dates = map(datetime.utcfromtimestamp, t*1000)
    dates = map(lambda x : float(x.strftime('%s')) + 1e-6*x.microsecond, dates)
    s = sin(0.1*pi*t)*exp(-t*0.01)
    
    ax = subplot(111)
    plot(dates,s)
    
    ax.xaxis.set_major_formatter(majorFormatter)
    
    xticks(rotation = 45)
    
    show()

if __name__ == "__main__":
    # Some unit tests
    if SmartPrinter(timedelta(days = 400)) != YearsPrinter:
        exit("400 days != YearsPrinter");
    if SmartPrinter(timedelta(days = 60)) != DayOfYearPrinter:
        exit("60 days != DayOfYearPrinter");
    if SmartPrinter(timedelta(seconds = 3600)) != MinutesPrinter:
        exit("1 hour != MinutesPrinter");
    if SmartPrinter(timedelta(seconds = 60)) != SecondsPrinter:
        exit("60 seconds != SecondsPrinter");
    if SmartPrinter(timedelta(milliseconds = 10)) != SecondsPrinter:
        exit("10 milliseconds != SecondsPrinter");
    
    # Run the demo:
    # demo()
