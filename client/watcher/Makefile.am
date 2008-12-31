include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-watcher


bionet_watcher_SOURCES = bionet-watcher.c

bionet_watcher_CFLAGS = -DBIONET_21_API $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libbionet -I../../util

bionet_watcher_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libbionet -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/client

bionet_watcher_LDADD = $(AM_LDADD) -lbionet-util-21 -lbionet-21 -lbionet-asn-21 -lcal-mdnssd-bip-client -lpthread -lm

