
# Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
# Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = pal-650

pal_650_SOURCES =             \
    pal-650-hab.c             \
    pal-connect.c             \
    pal-read.c                \
    pal-request-taglist.c     \
    cb-set-resource.c         \
    timeout-tags.c            \
    drop-node.c               \
    make-shutdowns-clean.c    \
    global.c

pal_650_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

pal_650_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(LDFLAGS_BIONET_HAB) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

pal_650_LDADD = $(AM_LDADD) -lpthread -lm $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER)

pal_650_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 
