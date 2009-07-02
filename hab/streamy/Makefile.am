
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = streamy-hab


streamy_hab_SOURCES =            \
    streamy-hab.c                \
    cb-stream-data.c             \
    cb-stream-subscription.c     \
    cb-stream-unsubscription.c   \
    publish-file.c               \
    make-incoming-node.c         \
    make-outgoing-node.c         \
    read-from-bionet.c

streamy_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

streamy_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

streamy_hab_LDADD = $(AM_LDADD) -lpthread -lm $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER)

streamy_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 
