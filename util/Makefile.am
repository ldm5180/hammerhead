
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
    bionet-event.h \
    bionet-util.h \
    bionet-node.h \
    bionet-stream.h \
    bionet-resource.h \
    bionet-datapoint.h \
    bionet-value.h \
    bionet-hab.h \
    bionet-platform.h \
    bionet-epsilon.h \
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
	$(GLIB_LIBS) \
	$(OPENSSL_LIBS)

libbionet_util_la_SOURCES =    \
    bdm-destructor.c           \
    bdm-free.c                 \
    bdm-hab-list.c             \
    bdm-incref.c               \
    bdm-is-secure.c            \
    bdm-matches.c              \
    bdm-new.c                  \
    bdm-node-uid.c             \
    bdm-user-data.c            \
    event-get.c                \
    event-alloc.c              \
    event-incref.c             \
    event-user-data.c          \
    epsilon-free.c             \
    epsilon-get.c              \
    epsilon-incref.c           \
    epsilon-new.c              \
    epsilon-to-string.c        \
    epsilon-user-data.c        \
    hab-add-node.c             \
    hab-destructor.c           \
    hab-event.c                \
    hab-free.c                 \
    hab-get-node.c             \
    hab-incref.c               \
    hab-matches.c              \
    hab-name.c                 \
    hab-new.c                  \
    hab-remove-node.c          \
    hab-user-data.c            \
    hab-is-secure.c            \
    node-add-resource.c        \
    node-add-stream.c          \
    node-destructor.c          \
    node-event.c               \
    node-free.c                \
    node-get-resource.c        \
    node-get-stream.c          \
    node-incref.c              \
    node-matches.c             \
    node-name.c                \
    node-new.c                 \
    node-sort-resource.c       \
    node-user-data.c           \
    resource-to-string.c       \
    resource-data-type.c       \
    resource-datapoint.c       \
    resource-destructor.c      \
    resource-flavor.c          \
    resource-free.c            \
    resource-get.c             \
    resource-get-datapoint.c   \
    resource-get-delta.c       \
    resource-get-epsilon.c     \
    resource-get-value.c       \
    resource-incref.c          \
    resource-is-dirty.c        \
    resource-make-clean.c      \
    resource-matches.c         \
    resource-name.c            \
    resource-new.c             \
    resource-persist.c         \
    resource-set.c             \
    resource-set-epsilon-delta.c \
    resource-user-data.c       \
    datapoint-destructor.c     \
    datapoint-dup.c            \
    datapoint-new.c            \
    datapoint-free.c	       \
    datapoint-event.c	       \
    datapoint-value.c          \
    datapoint-timestamp.c      \
    datapoint-is-dirty.c       \
    datapoint-make-clean.c     \
    datapoint-user-data.c      \
    datapoint-incref.c         \
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
    stream-incref.c                     \
    value-as-string.c                   \
    value-check-epsilon-delta.c         \
    value-free.c                        \
    value-get.c                         \
    value-get-resource.c                \
    value-incref.c                      \
    value-new.c                         \
    value-set.c                         \
    value-to-string.c                   \
    value-user-data.c                   \
    version.c


libbionet_util_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(BIONET_INTERFACE_VERSION)
