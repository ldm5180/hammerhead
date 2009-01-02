include $(top_srcdir)/common.am

bin_PROGRAMS = random-hab


random_hab_SOURCES =               \
    add-node.c                     \
    get-random-word.c              \
    pick-random-node.c             \
    random-hab.c                   \
    set-random-resource-value.c    \
    update-node.c                  \
    usage.c

random_hab_CFLAGS = -DBIONET_21_API $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

random_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

random_hab_LDADD = $(AM_LDADD) -lhab-21 -lbionet-util-21 -lbionet-asn-21 -lcal-mdnssd-bip-server -lpthread -lm

