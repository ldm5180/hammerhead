
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet.la

include_HEADERS = bionet.h libbionet-decl.h

libbionet_la_SOURCES =        \
    set-id.c                  \
    connect.c                 \
    disconnect.c	      \
    read.c                    \
    register-callback.c       \
    subscribe-datapoints.c    \
    subscribe-hab-list.c      \
    subscribe-node-list.c     \
    subscribe-stream.c        \
    unsubscribe-hab-list.c    \
    unsubscribe-node-list.c   \
    unsubscribe-datapoints.c  \
    unsubscribe-stream.c      \
    set-resource.c            \
    stream-write.c            \
    cal-callback.c            \
    cache-get-hab.c           \
    cache-cleanup.c           \
    cache-internal.c          \
    cache-lookup-hab.c        \
    cache-lookup-node.c       \
    cache-lookup-resource.c   \
    cache-lookup-stream.c     \
    global.c \
    security.c

libbionet_la_CPPFLAGS = $(AM_CPPFLAGS) -DBUILDING_LIBBIONET
libbionet_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CFLAGS_BIONET_CAL_UTIL) \
	$(CFLAGS_BIONET_CAL_CLIENT) \
	$(CFLAGS_BIONET_UTIL) \
	$(CFLAGS_BIONET_ASN1) \
	$(OPENSSL_CFLAGS) \
	-fno-strict-aliasing

libbionet_la_LIBADD = \
	$(AM_LDADD) \
	$(AVAHI_COMPAT_LIBDNS_SD_LIBS) \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(OPENSSL_LIBS) \
	$(LIBS_BIONET_CAL_UTIL) \
	$(LIBS_BIONET_CAL_CLIENT) \
	$(LIBS_BIONET_ASN1)  \
	$(LIBS_BIONET_UTIL)

libbionet_la_LDFLAGS = \
	$(AM_LDFLAGS) -version-info \
	$(BIONET_INTERFACE_VERSION) \
	$(LDFLAGS_BIONET_CAL_UTIL) \
	$(LDFLAGS_BIONET_CAL_CLIENT) \
	$(LDFLAGS_BIONET_UTIL) \
	$(LDFLAGS_BIONET_ASN1)

libbionet_la_DEPENCENDIES = \
	$(INCLUDES_BIONET_CLIENT) \
	$(INCLUDES_BIONET_CAL_UTIL) \
	$(INCLUDES_BIONET_UTIL) \
	$(INCLUDES_BIONET_ASN1) \
	$(CFLAGS_BIONET_CAL_CLIENT)

