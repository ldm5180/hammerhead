include $(top_srcdir)/common.am

LIBCAL_EVENT_SOURCES=../../util/event-free.c ../../util/event-new.c ../../util/peer-free.c ../../util/peer-new.c ../shared/bip-peer-free.c ../shared/bip-net-free.c
LIBCAL_MDNSSD_BIP_SHARED_SOURCES=

lib_LTLIBRARIES = libcal-mdnssd-bip-client.la

libcal_mdnssd_bip_client_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(DBUS_CFLAGS) -I.. -I../.. -I../../util
libcal_mdnssd_bip_client_la_SOURCES = cal-client-api.c global.c thread-functions.c $(LIBCAL_MDNSSD_BIP_SHARED_SOURCES) $(LIBCAL_EVENT_SOURCES)
libcal_mdnssd_bip_client_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)
