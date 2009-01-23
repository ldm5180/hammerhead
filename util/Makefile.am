
include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet-util.la

libbionet_util_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I ../asn.1

libbionet_util_la_LIBADD = $(AM_LDADD) $(GLIB20_LIBS)

libbionet_util_la_SOURCES =    \
    hab-add-node.c             \
    hab-remove-node.c          \
    hab-matches.c              \
    hab-new.c                  \
    hab-free.c                 \
    hab-get-node.c             \
    hab-name.c                 \
    node-new.c                 \
    node-add-resource.c        \
    node-get-resource.c        \
    node-add-stream.c          \
    node-get-stream.c          \
    node-matches.c             \
    node-free.c                \
    node-user-data.c           \
    node-name.c                \
    resource-new.c             \
    resource-free.c            \
    resource-get.c             \
    resource-get-value.c       \
    resource-data-type.c       \
    resource-flavor.c          \
    resource-datapoint.c       \
    resource-matches.c         \
    resource-is-dirty.c        \
    resource-make-clean.c      \
    resource-get-datapoint.c   \
    resource-set.c             \
    resource-name.c            \
    datapoint-new.c            \
    datapoint-free.c	\
    datapoint-value.c          \
    datapoint-timestamp.c      \
    datapoint-is-dirty.c       \
    datapoint-make-clean.c     \
    is-valid-name-component.c  \
    glib-log-handler.c                  \
    network-error.c                     \
    split-names.c \
    stream-direction.c                  \
    stream-type.c                       \
    stream-id.c                         \
    stream-free.c                       \
    stream-new.c                        \
    stream-name.c                       \
    stream-user-data.c                  \
    stream-node.c                       \
    stream-hab.c                        \
    value-free.c \
    value-get.c \
    value-get-resource.c \
    value-new.c \
    value-set.c \
    value-to-string.c 


#    stream-accept.c
#    stream-connect.c
#    stream-listen.c


libbionet_util_la_LDFLAGS = -version-info $(BIONET_INTERFACE_VERSION)
