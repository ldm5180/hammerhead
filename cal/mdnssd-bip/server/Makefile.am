include $(top_srcdir)/common.am

LIBCAL_EVENT_SOURCES=../../util/event-free.c ../../util/event-new.c

LIBCAL_MDNSSD_BIP_SHARED_SOURCES =           \
    ../shared/bip-net-clear.c                \
    ../shared/bip-net-destroy.c              \
    ../shared/bip-net-connect.c              \
    ../shared/bip-net-disconnect.c           \
    ../shared/bip-net-free.c                 \
    ../shared/bip-net-is-connected.c         \
    ../shared/bip-net-new.c                  \
    ../shared/bip-peer-free.c                \
    ../shared/bip-peer-connect.c             \
    ../shared/bip-peer-disconnect.c          \
    ../shared/bip-peer-get-connected-net.c   \
    ../shared/bip-peer-new.c                 \
    ../shared/bip-read-from-peer.c           \
    ../shared/bip-send-message.c

lib_LTLIBRARIES = libcal-mdnssd-bip-server.la

libcal_mdnssd_bip_server_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(DBUS_CFLAGS) -I.. -I../.. -I../../util
libcal_mdnssd_bip_server_la_SOURCES = cal-server-api.c global.c thread-functions.c $(LIBCAL_MDNSSD_BIP_SHARED_SOURCES) $(LIBCAL_EVENT_SOURCES)
libcal_mdnssd_bip_server_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)

libcal_mdnssd_bip_server_la_LDFLAGS = -version-info $(BIONET_INTERFACE_VERSION)
