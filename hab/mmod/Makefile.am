include $(top_srcdir)/common.am

if TINYOSMIG_DISABLED
ADD =
else
ADD = mmodaccelmsg mmodgenmsg mmodsettingsmsg
BUILT_SOURCES = mmodaccelmsg.c mmodgenmsg.c mmodsettingsmsg.c 
endif

bin_PROGRAMS = mmod-hab

mmod_hab_DEPENDENCIES = $(ADD)

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


mmodaccelmsg: mmod_node/mmod_msg.h
	touch mmodaccelmsg.c
	mig -o mmodaccelmsg.h -c-prefix=MMODACCELMSG c mmod_node/mmod_msg.h mmod_accel_msg

mmodgenmsg: mmod_node/mmod_msg.h
	touch mmodgenmsg.c
	mig -o mmodgenmsg.h -c-prefix=MMODGENMSG c mmod_node/mmod_msg.h mmod_general_msg

mmodsettingsmsg: mmod_node/mmod_msg.h
	touch mmodsettingsmsg.c
	mig -o mmodsettingsmsg.h -c-prefix=MMODSETTINGSMSG c mmod_node/mmod_msg.h mmod_settings_msg
