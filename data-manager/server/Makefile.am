include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-data-manager


bionet_data_manager_SOURCES =     \
    bionet-data-manager.c         \
    bionet-io.c                   \
    db.c                          \
    client-connecting-handler.c   \
    client-readable-handler.c     \
    handle-client-message.c       \
    disconnect-client.c           \
    make-listening-socket.c       \
    make-shutdowns-clean.c        \
    keepalive.c

bionet_data_manager_CFLAGS = $(AM_CFLAGS) $(OPENSSL_CFLAGS) $(GLIB20_CFLAGS) $(OPENSSL_CFLAGS) -I../include -I../../client/libbionet -I../../util -I../../asn.1

bionet_data_manager_LDFLAGS = $(AM_LDFLAGS) $(OPENSSL_LIBS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../../client/libbionet -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/client

bionet_data_manager_LDADD = $(AM_LDADD) -lsqlite3 -lbionet -lbionet-util -lbionet-asn -lcal-mdnssd-bip-client -lpthread -lm

bionet_data_manager_DEPENDENCIES = ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la ../../cal/mdnssd-bip/client/libcal-mdnssd-bip-client.la ../../client/libbionet/libbionet.la