
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

if ENABLE_PYTHON
SUBDIRS = python
endif

lib_LTLIBRARIES = libbionet-util.la

include_HEADERS = \
    libbionet-util-decl.h \
    bionet-bdm.h \
    bionet-util.h \
    bionet-node.h \
    bionet-stream.h \
    bionet-resource.h \
    bionet-datapoint.h \
    bionet-value.h \
    bionet-hab.h \
    bionet-platform.h \
    bionet-version.h


libbionet_util_la_CPPFLAGS = $(AM_CPPFLAGS) -DBUILDING_LIBBIONET_UTIL
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
    bdm-add-hab.c              \
    bdm-get-hab.c              \
    bdm-new.c                  \
    bdm-free.c                 \
    bdm-matches.c              \
    bdm-remove-hab.c           \
    delta-free.c               \
    delta-get.c                \
    delta-new.c                \
    delta-to-string.c          \
    hab-add-bdm.c              \
    hab-add-node.c             \
    hab-remove-bdm.c           \
    hab-remove-node.c          \
    hab-matches.c              \
    hab-new.c                  \
    hab-free.c                 \
    hab-get-bdm.c              \
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
    resource-persist.c         \
    resource-make-clean.c      \
    resource-get-datapoint.c   \
    resource-get-delta.c       \
    resource-get-epsilon.c     \
    resource-set.c             \
    resource-set-delta-epsilon.c \
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
    split-names-bdm.c \
    stream-direction.c                  \
    stream-type.c                       \
    stream-id.c                         \
    stream-free.c                       \
    stream-new.c                        \
    stream-name.c                       \
    stream-user-data.c                  \
    stream-node.c                       \
    stream-hab.c                        \
    value-check-delta-epsilon.c         \
    value-free.c                        \
    value-get.c                         \
    value-get-resource.c                \
    value-new.c                         \
    value-set.c                         \
    value-to-string.c                   \
    version.c


libbionet_util_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(BIONET_INTERFACE_VERSION)
