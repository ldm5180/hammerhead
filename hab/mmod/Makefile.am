include $(top_srcdir)/common.am

bin_PROGRAMS = mmod-hab


mmod_hab_SOURCES =    \
    daemonize.c       \
    gateway.c         \
    heartbeat.c       \
    main.c            \
    message.c         \
    mmodaccelmsg.c    \
    mmodgenmsg.c      \
    mmod_message.c    \
    mmodsettingsmsg.c \
    mts310_cook.c     \
    serialsource.c    \
    set-resource.c

mmod_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

mmod_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

mmod_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm

