
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.

include $(top_srcdir)/common.am

bin_PROGRAMS = bpstats2-hab
ION_CFLAGS=-L/usr/lib
ION_LIBS= \
	-lbp \
	-lbpP \
	-lici \
	-lrfx

bpstats2_hab_SOURCES = bpstats2-hab.c	\
	usage.c								\
	handle-hab-read.c					\
	handle-bundles.c

bpstats2_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) $(ION_CFLAGS)

bpstats2_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

bpstats2_hab_LDADD = $(AM_LDADD) -lpthread -lm $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER) $(ION_LIBS)

bpstats2_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL)
