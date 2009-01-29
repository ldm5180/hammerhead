include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet.la

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
    stream-write.c            \
    cal-callback.c            \
    cache-get-hab.c           \
    cache-internal.c          \
    cache-lookup-hab.c        \
    cache-lookup-node.c       \
    cache-lookup-resource.c   \
    cache-lookup-stream.c     \
    global.c

libbionet_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_CAL_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1)
libbionet_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)

libbionet_la_LDFLAGS = -version-info $(BIONET_INTERFACE_VERSION)

libbionet_la_DEPENCENDIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_CAL_UTIL) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1) $(CFLAGS_BIONET_CAL_CLIENT)

