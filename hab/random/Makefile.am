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

random_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

random_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../util -L../../cal/mdnssd-bip/server

random_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lcal-mdnssd-bip-server -lpthread -lm

