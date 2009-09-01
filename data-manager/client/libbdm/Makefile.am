
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbdm.la

include_HEADERS = bdm-client.h

libbdm_la_SOURCES =             \
    read.c \
    cal-callback.c \
    connect.c                   \
    disconnect.c                \
    get-resource-datapoints.c   \
    global.c   \
    register-callback.c \
    send-asn.c                  

libbdm_la_CFLAGS = $(AM_CFLAGS) $(CFLAGS_BDM) -I$(top_srcdir)/data-manager/include $(CFLAGS_BIONET_CAL_CLIENT) $(CFLAGS_BIONET_CAL_UTIL)

libbdm_la_LDFLAGS = $(AM_LDFLAGS) $(LDFLAGS_BDM) $(LDFLAGS_BIONET_CAL_CLIENT)

libbdm_la_LIBADD = $(AM_LDADD) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT) -lpthread -lm
