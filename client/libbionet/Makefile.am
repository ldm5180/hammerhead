include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet.la libbionet-21.la

libbionet_la_SOURCES =        \
    set-id.c                  \
    connect.c                 \
    read.c                    \
    register-callback.c       \
    subscribe-datapoints.c    \
    subscribe-hab-list.c      \
    subscribe-node-list.c     \
    subscribe-stream.c        \
    set-resource.c            \
    cal-callback.c            \
    cache-get-hab.c           \
    cache-internal.c          \
    cache-lookup-hab.c        \
    cache-lookup-node.c       \
    cache-lookup-resource.c   \
    cache-lookup-stream.c     \
    global.c

libbionet_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../../cal -I ../../cal/util -I../../util -I../../asn.1
libbionet_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)

libbionet_21_la_SOURCES =        \
    set-id.c                  \
    connect.c                 \
    read.c                    \
    register-callback.c       \
    subscribe-datapoints.c    \
    subscribe-hab-list-2.1.c      \
    subscribe-node-list.c     \
    subscribe-stream.c        \
    set-resource-2.1.c            \
    cal-callback.c            \
    cache-get-hab.c           \
    cache-internal.c          \
    cache-lookup-hab.c        \
    cache-lookup-node.c       \
    cache-lookup-resource.c   \
    cache-lookup-stream.c     \
    global.c

libbionet_21_la_CFLAGS = -DBIONET_21_API $(AM_CFLAGS) $(GLIB20_CFLAGS) -I. -I../../cal -I ../../cal/util -I../../util -I../../asn.1
libbionet_21_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS) -lbionet-util-21
libbionet_21_la_LDFLAGS = -L../../util

