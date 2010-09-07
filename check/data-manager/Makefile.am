
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

TESTS = check-data-manager

check_PROGRAMS = check-data-manager

nodist_check_data_manager_SOURCES = \
	../check-common.c \
	check-bdm.c \
	check-bdm-cfg.c \
	check-bdm-subscription-topic.c \
	check-bdm-sync.c

check_data_manager_CFLAGS = $(AM_CFLAGS) $(CHECK_CFLAGS) \
    -I.. \
    -I$(top_srcdir)/data-manager/include \
    $(CFLAGS_BIONET_CAL_SERVER) \
    $(CFLAGS_BIONET_CAL_UTIL) \
    $(CFLAGS_BIONET_CLIENT) \
    $(CFLAGS_BDM) \
    $(CFLAGS_BIONET_ASN1) \
    $(CFLAGS_BIONET_UTIL) \
    $(GLIB_CFLAGS)

check_data_manager_LDFLAGS = $(AM_LDFLAGS) $(CHECK_LIBS) \
    $(LDFLAGS_BIONET_CLIENT) \
    $(LDFLAGS_BDM) \
    $(LDFLAGS_BIONET_ASN1) \
    $(GLIB_LIBS)

check_data_manager_LDADD = \
    $(LIBS_BIONET_CLIENT) \
    $(LIBS_BIONET_HAB) \
    $(LIBS_BDM) \
    $(LIBS_BIONET_UTIL) \
    $(top_srcdir)/data-manager/server/libbionet-data-manager.la

