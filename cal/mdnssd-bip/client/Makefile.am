
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libcal-mdnssd-bip-client.la

libcal_mdnssd_bip_client_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(OPENSSL_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) \
	$(DBUS_CFLAGS) \
	-I.. \
	$(CFLAGS_BIONET_CAL_CLIENT) \
	$(CFLAGS_BIONET_CAL_UTIL) \
	$(OPENSSL_CFLAGS)

libcal_mdnssd_bip_client_la_SOURCES = \
	cal-client-api.c \
	bip-net-connect.c                  \
	bip-peer-connect.c                  \
	global.c \
	thread-functions.c


libcal_mdnssd_bip_client_la_LIBADD = \
	$(AM_LDADD) \
	$(LIBS_CAL_MDNSSD_BIP_SHARED) \
	$(LIBS_CAL_UTIL) \
	$(AVAHI_COMPAT_LIBDNS_SD_LIBS) \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(OPENSSL_LIBS)

libcal_mdnssd_bip_client_la_LDFLAGS = $(AM_LDFLAGS) \
	-version-info \
	$(BIONET_INTERFACE_VERSION) 
