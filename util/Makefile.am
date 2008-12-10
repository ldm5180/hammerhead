
include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet-util.la

libbionet_util_la_SOURCES =    \
    hab-add-node.c             \
    hab-remove-node.c          \
    hab-matches.c              \
    hab-new.c                  \
    hab-free.c                 \
    hab-get-node.c             \
    node-new.c                 \
    node-add-resource.c        \
    node-get-resource.c        \
    node-add-stream.c          \
    node-get-stream.c          \
    node-matches.c             \
    node-free.c                \
    resource-new.c             \
    resource-free.c            \
    resource-data-type.c       \
    resource-flavor.c          \
    resource-datapoint.c       \
    resource-matches.c         \
    resource-is-dirty.c        \
    resource-make-clean.c      \
    resource-get-datapoint.c   \
    resource-set.c             \
    datapoint-new.c            \
    datapoint-value.c          \
    datapoint-timestamp.c      \
    datapoint-is-dirty.c       \
    datapoint-make-clean.c     \
    stream-accept.c                     \
    stream-connect.c                    \
    stream-direction.c                  \
    stream-free.c                       \
    stream-listen.c                     \
    stream-new.c                        \
    stream-port.c                       \
    stream-set-host.c                   \
    is-valid-name-component.c  \
    glib-log-handler.c                  \
    network-error.c                     \
    split-names.c

libbionet_util_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I ../asn.1

libbionet_util_la_LIBADD = $(AM_LDADD) $(GLIB20_LIBS)

