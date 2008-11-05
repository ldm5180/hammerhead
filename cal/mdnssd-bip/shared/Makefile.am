include $(top_srcdir)/common.am

lib_LTLIBRARIES = libcal-mdnssd-bip-shared.la

libcal_mdnssd_bip_shared_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(DBUS_CFLAGS) -I.. -I../.. -I../../util
libcal_mdnssd_bip_shared_la_SOURCES = bip-read-from-peer.c bip-send-message.c bip-net-free.c bip-peer-free.c bip-net-clear.c
libcal_mdnssd_bip_shared_la_LIBADD = $(AM_LDADD) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_CFLAGS) 
