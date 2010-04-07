
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-watcher


bionet_watcher_SOURCES = bionet-watcher.c bionet-callbacks.c bdm-callbacks.c timestamps.c

bionet_watcher_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CFLAGS_BIONET_CLIENT) \
	$(CFLAGS_BIONET_UTIL) \
	$(CFLAGS_BDM)

bionet_watcher_LDFLAGS = \
	$(AM_LDFLAGS) \
	$(GLIB_LIBS) \
	$(LDFLAGS_BIONET_CLIENT) \
	$(LDFLAGS_BIONET_UTIL) \
	$(LDFLAGS_BDM) 

bionet_watcher_LDADD = \
	$(AM_LDADD) \
	$(LIBS_BIONET_CLIENT) \
	$(LIBS_BIONET_UTIL) \
	$(LIBS_BDM) \
	-lpthread \
	-lm 

