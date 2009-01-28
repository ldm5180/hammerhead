include $(top_srcdir)/common.am

bin_PROGRAMS = streamy-hab


streamy_hab_SOURCES =            \
    streamy-hab.c                \
    cb-stream-data.c             \
    cb-stream-subscription.c     \
    cb-stream-unsubscription.c   \
    publish-file.c               \
    make-incoming-node.c         \
    make-outgoing-node.c         \
    read-from-bionet.c

streamy_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

streamy_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

streamy_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm

streamy_hab_DEPENDENCIES = ../libhab/libhab.la ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la ../../cal/mdnssd-bip/server/libcal-mdnssd-bip-server.la