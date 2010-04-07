
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = random-hab


random_hab_SOURCES =               \
    add-node.c                     \
    get-random-word.c              \
    pick-random-node.c             \
    random-hab.c                   \
    set-random-resource-value.c    \
    update-node.c                  \
    usage.c

random_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

random_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

random_hab_LDADD = $(AM_LDADD) $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) -lm

random_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 
