
include $(top_srcdir)/common.am

bin_PROGRAMS = syshealth-hab


syshealth_hab_SOURCES =      \
    syshealth.c              \
    disk-free.c              \
    mem-free.c               \
    uptime.c                 \
    load-average.c           \
    smart.c                  \
    sensors.c                \
    scheduling-latency.c


syshealth_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

syshealth_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

syshealth_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm

syshealth_hab_DEPENDENCIES = ../libhab/libhab.la ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la ../../cal/mdnssd-bip/server/libcal-mdnssd-bip-server.la
