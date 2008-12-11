include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-stream


bionet_stream_SOURCES = bionet-stream.c

bionet_stream_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libbionet -I../../util

bionet_stream_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libbionet -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/client

bionet_stream_LDADD = $(AM_LDADD) -lbionet -lbionet-util -lbionet-asn -lcal-mdnssd-bip-client -lpthread -lm

