
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libamedevice.la

libamedevice_la_SOURCES =      \
	amedevice.c            \
	sum8.c      


libamedevice_la_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(BLUEZ_CFLAGS) $(CFLAGS_BIONET_CAL_SERVER) $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) 
libamedevice_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB_LIBS) $(DBUS_LIBS) $(BLUEZ_LIBS)


libamedevice_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(BIONET_INTERFACE_VERSION) 

libamedevice_la_DEPENDENCIES = $(INCLUDES_BIONET_CAL_SERVER) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1)
