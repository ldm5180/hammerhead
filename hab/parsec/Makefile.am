include $(top_srcdir)/common.am

bin_PROGRAMS = parsec-hab


parsec_hab_SOURCES =               \
    main.c                         \
    read_parsec.c                  \
    expire-old-nodes.c             \
    daemonize.c

parsec_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

parsec_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

parsec_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm
