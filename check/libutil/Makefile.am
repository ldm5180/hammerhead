
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

TESTS = check-libutil

check_PROGRAMS = check-libutil

nodist_check_libutil_SOURCES = \
	../check-common.c \
	check-libutil.c \
	check-libutil-hab-tests.c \
	check-libutil-split-hab-name.c

check_libutil_CFLAGS = $(CHECK_CFLAGS) -I.. $(CFLAGS_BIONET_UTIL) $(GLIB20_CFLAGS)

check_libutil_LDFLAGS = $(CHECK_LIBS) $(LDFLAGS_BIONET_UTIL) $(GLIB20_LIBS)

check_libutil_LDADD = $(LIBS_BIONET_UTIL)

