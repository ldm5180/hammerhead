include $(top_srcdir)/common.am

bin_PROGRAMS = bdm-client

bdm_client_SOURCES =            \
    bdm-client.c                \
    connect.c                   \
    disconnect.c                \
    get-resource-datapoints.c   \
    send-asn.c

bdm_client_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../include -I../../util -I../../asn.1

bdm_client_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../../asn.1 -L../../util

bdm_client_LDADD = $(AM_LDADD) -lbionet-util -lbionet-asn -lpthread -lm

bdm_client_DEPENDENCIES = ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la
