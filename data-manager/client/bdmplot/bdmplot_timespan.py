#!/usr/bin/python
# A timespan class and associated parsers.

from datetime import timedelta, datetime
from calendar import timegm
import time, os, re

# All Bionet times are in UTC.  All users should enter times in UTC.
os.environ[ 'TZ' ] = "UTC"
time.tzset()

def timeval_to_float(tv, evaluateNow = False):
    """
    Converts from a struct timeval to a timeval-as-float; i.e. a
    struct timeval tv = { 32, 56000 } would become 32.056000
    """
    if tv == None:
        if evaluateNow == False:
            return None
        else:
            now = time.time()
            return timeval_to_float(double_to_timeval((int(now), (now % 1)*1e6)))
    if getattr(tv, '__iter__', None) == None:
        return (tv.tv_sec) + (1e-6*tv.tv_usec)
    else:
        return map(timeval_to_float, tv)

def timeval_to_int(tv, evaluateNow = False):
    """
    Converts from a struct timeval to a timeval-as-float; i.e. a
    struct timeval tv = { 32, 56000 } would become 32.056000
    """
    if tv == None:
        if evaluateNow == False:
            return None
        else:
            now = time.time()
            return int(timeval_to_float(double_to_timeval((int(now), (now % 1)*1e6))))
    if getattr(tv, '__iter__', None) == None:
        return int((tv.tv_sec) + (1e-6*tv.tv_usec))
    else:
        return int(map(timeval_to_float, tv))

def timespan_to_timedelta(s):
    """Converts strings like "24d" or "1w 3d 23h 14.23s" to timedeltas."""
    ts = timedelta()
    while len(s) > 0:
        m = re.match(r'([0-9\.]+)([ywdhms])\s*(.*)', s)
        try:
            ts = ts + { 'y' : lambda x : timedelta(years=x),
                        'w' : lambda x : timedelta(weeks=x),
                        'd' : lambda x : timedelta(days=x),
                        'h' : lambda x : timedelta(hours=x),
                        'm' : lambda x : timedelta(minutes=x),
                        's' : lambda x : timedelta(seconds=x) } \
                            [m.group(2)](float(m.group(1)))
        except:
            print "Unhandled time increment " + m.group(2)
            return None
        s = m.group(3)
    return ts

def pystr_to_time(str, now = None):
    str = str.lstrip().rstrip()
    if now == None:
        now = datetime.now()
    if str.lstrip() == "now":
        return (int(now.strftime("%s")), now.microsecond)
    else:
        strsplit = str.split(".")
    try:
        time_s = time.strftime("%s", time.strptime(strsplit[0], "%Y-%m-%d %H:%M:%S"))
    except:
        print "Timestamp %s doesn't obey format YY-MM-DD HH:MM:SS" % strsplit[0]
        return None
    try:
        time_us = int(float("0." + strsplit[1])*1e6)
    except:
        time_us = 0
    return (time_s, time_us)


# Timevals as used for this project are currently defined in the bdm_client
# module, but that module might not be available for other users.
try:
    from bdm_client import timeval
    def double_to_timeval(time_sandus):
        tv = timeval()
        tv.tv_sec = int(time_sandus[0])
        tv.tv_usec = int(time_sandus[1])
        return tv
    def pystr_to_timeval(str, now = None):
        return double_to_timeval(pystr_to_time(str, now))
    def timespan_to_timevals(timespan, now = None):
        """
        Converts strings like "24d" or "1w 3d 23h 14.23s" to a double of timevals, like:

            (now - timespan,  now)

        If now is not specified, it defaults to the current UTC time.
        """
        # If now is not specified, grab it
        if now == None:
            now = datetime.now()
        # If this is a "since" timespan, convert it:
        if timespan.startswith("since"):
            if timespan.endswith("ago"):
                # "since 24d ago" == "last 24d".
                timespan = "last " + timespan[6:-4]
            else:
                timespan = timespan[6:] + "to now"
        
        # If this is a "last" timespan, convert it
        if timespan.startswith("last"):
            td = timespan_to_timedelta(timespan[5:])
            start = (now - td)
            tv_start = double_to_timeval((int(start.strftime("%s")), start.microsecond))
            return (tv_start, None)
        
        # If we're still here, then this is a timespan of the form "X to Y"
        # where X is an absolute time and Y is either an absolute time or now.
        (startstr, stopstr) = timespan.split("to", 1)
        tv_start = pystr_to_timeval(startstr, now);
        tv_stop  = pystr_to_timeval(stopstr, now);
        if stopstr.lstrip().rstrip() == "now":
            return (tv_start, None)
        return (tv_start, tv_stop)
except:
    print "Couldn't import bdm_client, no timespan_to_timevals support"

# Timespan tests
if __name__ == "__main__":
    # Test timespan_to_timedelta
    x = timespan_to_timedelta("24d")
    if x != timedelta(days=24):
        exit("24 days test failed: %s" % repr(x))
    x = timespan_to_timedelta("1h 23m 14.23s")
    if x != timedelta(hours=1, minutes=23, seconds=14.23):
        exit("h/m/s.ms test failed: %s" % repr(x))
    
    # Test timespan to timevals
    try:
        # Use a known now.
        now = datetime(2009, 01, 03, 13, 24)
        y = timespan_to_timevals("since 24d ago", now)
        if y[0].tv_sec != 1228915440 or y[0].tv_usec != 0 or y[1].tv_sec != 1230989040 or y[1].tv_usec != 0:
            exit("Failed 24d ago test")
        y = timespan_to_timevals("last 13m 3.25s", now)
        if y[0].tv_sec != 1230988256 or y[0].tv_usec != 750000 or y[1].tv_sec != 1230989040 or y[1].tv_usec != 0:
            exit("Failed last 13m 3.25s test (%d.%06d to %d.%06d)" % 
                    (y[0].tv_sec, y[0].tv_usec, y[1].tv_sec, y[1].tv_usec))
        # Test explicit string to timevals
        # Andrew's birthday
        testtv = pystr_to_timeval("1984-04-14 18:50:00.2345")
        if testtv.tv_sec != 450816600 or testtv.tv_usec != 234500:
            exit("Andrew's birthday test failed (%d, %d)" % (testtv.tv_sec, testtv.tv_usec))
        # DTN "epoch" and Gregorian rollover
        testtv = pystr_to_timeval("2000-01-01 00:00:00.0000")
        if testtv.tv_sec != 946684800 or testtv.tv_usec != 0:
            exit("DTN epoch test failed (%d, %d)" % (testtv.tv_sec, testtv.tv_usec))
        # 100 seconds before the big time_t 32-bit rollover.
        testtv = pystr_to_timeval("2038-01-19 03:12:27.90")
        if testtv.tv_sec != 2147483547 or testtv.tv_usec != 900000:
            exit("close-to-time_t test failed (%d, %d)" % (testtv.tv_sec, testtv.tv_usec))
        
        # Test explicit timespans.
        z = timespan_to_timevals("2000-01-01 00:00:00.1234 to now", now)
        if z[0].tv_sec != 946684800 or z[0].tv_usec != 123400 or z[1].tv_sec != 1230989040 or z[1].tv_usec != 0:
            exit("Explicit timespan test 1 failed")
    except NameError:
        print "Warning: couldn't test timespan_to_timevals(). Install BioNet."

