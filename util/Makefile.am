
include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet-util.la #libbionet-util-21.la

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
    stream-direction.c                  \
    stream-free.c                       \
    stream-new.c                        \
    stream-name.c                       \
    is-valid-name-component.c  \
    glib-log-handler.c                  \
    network-error.c                     \
    split-names.c

#    stream-accept.c
#    stream-connect.c
#    stream-listen.c

libbionet_util_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I ../asn.1

libbionet_util_la_LIBADD = $(AM_LDADD) $(GLIB20_LIBS)




#libbionet_util_21_la_SOURCES =    \
    hab-add-node.c             \
    hab-remove-node.c          \
    hab-matches.c              \
    hab-new-2.1.c                  \
    hab-free.c                 \
    hab-get-node.c             \
    node-new-2.1.c                 \
    node-add-resource.c        \
    node-get-resource.c        \
    node-add-stream.c          \
    node-get-stream.c          \
    node-matches.c             \
    node-free.c                \
    resource-new-2.1.c             \
    resource-free-2.1.c            \
    resource-data-type.c       \
    resource-flavor.c          \
    resource-datapoint-2.1.c       \
    resource-matches.c         \
    resource-is-dirty.c        \
    resource-make-clean.c      \
    resource-get-datapoint.c   \
    resource-set-2.1.c             \
    datapoint-new-2.1.c            \
    datapoint-free-2.1.c	\
    datapoint-value-2.1.c          \
    datapoint-timestamp-2.1.c      \
    datapoint-is-dirty.c       \
    datapoint-make-clean.c     \
    is-valid-name-component.c  \
    glib-log-handler.c                  \
    network-error.c                     \
    split-names.c \
    value-free.c \
    value-get.c \
    value-get-resource.c \
    value-new.c \
    value-set.c \
    value-to-string.c 


#    stream-direction.c                  \
#    stream-free.c                       \
#    stream-new.c                        \
#    stream-accept.c
#    stream-connect.c
#    stream-listen.c

#libbionet_util_21_la_CFLAGS = -DBIONET_21_API $(AM_CFLAGS) $(GLIB20_CFLAGS) -I ../asn.1

#libbionet_util_21_la_LIBADD = $(AM_LDADD) $(GLIB20_LIBS)

