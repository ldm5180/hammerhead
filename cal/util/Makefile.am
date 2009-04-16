
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libevent.la

libevent_la_SOURCES = event-free.c event-new.c
libevent_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS)
libevent_la_LIBADD = $(GLIB20_LIBS)

libevent_la_LDFLAGS = -version-info $(BIONET_INTERFACE_VERSION) 

