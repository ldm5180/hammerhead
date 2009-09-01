
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet-util.la

include_HEADERS = \
    bionet-bdm.h \
    bionet-util.h \
    bionet-node.h \
    bionet-stream.h \
    bionet-resource.h \
    bionet-datapoint.h \
    bionet-value.h \
    bionet-hab.h \
    bionet-version.h


libbionet_util_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CFLAGS_BIONET_CAL_UTIL) \
	$(CFLAGS_BIONET_CAL_CLIENT) \
	$(CFLAGS_BIONET_CAL_SERVER) \
	$(CFLAGS_BIONET_ASN1)

libbionet_util_la_LIBADD = \
	$(AM_LDADD) \
	$(GLIB_LIBS) 

libbionet_util_la_SOURCES =    \
    bdm-new.c                  \
    hab-add-node.c             \
    hab-remove-node.c          \
    hab-matches.c              \
    hab-new.c                  \
    hab-free.c                 \
    hab-get-node.c             \
    hab-user-data.c            \
    hab-name.c                 \
    hab-is-secure.c            \
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
    resource-user-data.c       \
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
    value-to-string.c \
    version.c


libbionet_util_la_LDFLAGS = -version-info $(BIONET_INTERFACE_VERSION)
