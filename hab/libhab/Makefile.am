
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libhab.la

libhab_la_SOURCES =      \
    connect.c            \
    disconnect.c         \
    report-new-node.c    \
    report-lost-node.c   \
    report-datapoints.c  \
    publish-stream.c     \
    read.c               \
    register-callback.c  \
    cal-callback.c       \
    cal-topic-matches.c  \
    global.c

libhab_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_CAL_SERVER) $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) 
libhab_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)

libhab_la_LDFLAGS = -version-info $(BIONET_INTERFACE_VERSION) 

libhab_la_DEPENDENCIES = $(INCLUDES_BIONET_CAL_SERVER) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1)
