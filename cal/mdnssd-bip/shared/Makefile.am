
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libcal-mdnssd-bip-shared.la

libcal_mdnssd_bip_shared_la_SOURCES =   \
    bip-net-new.c                       \
    bip-net-clear.c                     \
    bip-net-is-connected.c              \
    bip-net-disconnect.c                \
    bip-net-free.c                      \
    bip-net-destroy.c                   \
    bip-read-from-peer.c                \
    bip-send-message.c                  \
    bip-peer-new.c                      \
    bip-peer-disconnect.c               \
    bip-peer-get-connected-net.c        \
    bip-peer-free.c                     \
    bip-security.c			\
    bip-shared-cfg.c			\
    bip-peer-is-secure.c                \
    $(SYSAPI)/bip-itc.c                 \
    $(SYSAPI)/bip-socket.c

libcal_mdnssd_bip_shared_la_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(DBUS_CFLAGS) -I.. -I../.. -I../../util $(OPENSSL_CFLAGS)
libcal_mdnssd_bip_shared_la_LIBADD = $(AM_LDADD) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_CFLAGS) $(OPENSSL_LIBS)

libcal_mdnssd_bip_shared_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(BIONET_INTERFACE_VERSION) 

