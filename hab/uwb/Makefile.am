include $(top_srcdir)/common.am

bin_PROGRAMS = td-p210-hab


td_p210_hab_SOURCES =               \
    td-radio-hab.c                  \
    global.c                        \
    process-data.c                  \
    td-radio-connect.c              \
    td-radio-read.c                 \
    make-shutdowns-clean.c          \
    cb-set-resource.c


td_p210_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

td_p210_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

td_p210_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm
