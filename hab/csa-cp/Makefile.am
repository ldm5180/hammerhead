include $(top_srcdir)/common.am

bin_PROGRAMS = csa-cp-hab


csa_cp_hab_SOURCES =         \
    clear-csa.c              \
    csa-cp-hab.c             \
    download-session.c       \
    get-num-files.c          \
    make-shutdowns-clean.c   \
    ping.c                   \
    print-help.c             \
    probe.c                  \
    serial.c                 \
    set-resource.c


csa_cp_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

csa_cp_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

csa_cp_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm

csa_cp_hab_DEPENDENCIES = ../libhab/libhab.la ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la ../../cal/mdnssd-bip/server/libcal-mdnssd-bip-server.la