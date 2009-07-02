
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bdm-client

bdm_client_SOURCES =            \
    bdm-client.c                \
    connect.c                   \
    disconnect.c                \
    get-resource-datapoints.c   \
    send-asn.c

bdm_client_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) -I../include -fno-strict-aliasing

bdm_client_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1)

bdm_client_LDADD = $(AM_LDADD) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) -lpthread -lm

bdm_client_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1)
