#!/usr/bin/python
# timechooser.py
# Chooses a reasonable format for plotting against a time axis.

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
