include $(top_srcdir)/common.am

bin_PROGRAMS = uwb-hab


uwb_hab_SOURCES =               \
    main.c                          \
    global.c                        \
    process-data.c                  \
    uwb-connect.c                   \
    uwb-read.c                      \
    make-shutdowns-clean.c          \
    cb-set-resource.c


uwb_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

uwb_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

uwb_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm
