
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

TESTS = check-data-manager

check_PROGRAMS = check-data-manager

nodist_check_data_manager_SOURCES = \
	../check-common.c \
	check-bdm.c \
	check-bdm-cfg.c

check_data_manager_CFLAGS = $(CHECK_CFLAGS) \
    -I.. $(CFLAGS_BIONET_CLIENT) \
    $(CFLAGS_BIONET_ASN1) \
    $(CFLAGS_BIONET_UTIL) \
    $(GLIB20_CFLAGS)

check_data_manager_LDFLAGS = $(CHECK_LIBS) \
    $(LDFLAGS_BIONET_CLIENT) \
    $(LDFLAGS_BIONET_ASN1) \
    $(GLIB20_LIBS)

check_data_manager_LDADD = \
    $(LIBS_BIONET_CLIENT) \
    $(LIBS_BIONET_UTIL) \
    $(top_srcdir)/data-manager/server/libbionet-data-manager.la

