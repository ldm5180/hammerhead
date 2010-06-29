
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am


lib_LTLIBRARIES = libcal-mdnssd-bip-server.la

libcal_mdnssd_bip_server_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(OPENSSL_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GTHREAD_CFLAGS) \
	$(DBUS_CFLAGS) \
	-I.. -I../.. -I../../util -I../shared \
	$(AVAHI_COMPAT_LIBDNS_SD_CFLAGS)

libcal_mdnssd_bip_server_la_SOURCES = \
	cal-server-api.c \
	global.c \
	thread-functions.c

libcal_mdnssd_bip_server_la_LIBADD = \
	$(AM_LDADD) \
	$(LIBS_CAL_MDNSSD_BIP_SHARED) \
	$(LIBS_CAL_UTIL) \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(OPENSSL_LIBS) \
	$(GTHREAD_LIBS) \
	$(AVAHI_COMPAT_LIBDNS_SD_LIBS)

libcal_mdnssd_bip_server_la_LDFLAGS = \
	$(AM_LDFLAGS) \
	-version-info \
	$(BIONET_INTERFACE_VERSION)
