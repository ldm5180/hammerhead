include $(top_srcdir)/common.am

lib_LTLIBRARIES = libcal-mdnssd-bip-client.la

libcal_mdnssd_bip_client_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(DBUS_CFLAGS) -I.. -I../.. -I../../util
libcal_mdnssd_bip_client_la_SOURCES = cal-client-api.c global.c thread-functions.c
libcal_mdnssd_bip_client_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS) ../shared/libcal-mdnssd-bip-shared.la ../../util/libpeer.la ../../util/libevent.la
