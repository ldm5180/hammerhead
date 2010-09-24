
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

TESTS = check-bps

check_PROGRAMS = check-bps

nodist_check_bps_SOURCES = \
	check-bps.c \
	bps-api-tests.c

check_bps_CFLAGS = $(AM_CFLAGS) $(CHECK_CFLAGS) $(GLIB_CFLAGS) -fno-strict-aliasing

check_bps_LDFLAGS = $(CHECK_LDFLAGS) $(GLIB_LDFLAGS)

check_bps_LDADD = $(CHECK_LIBS) $(GLIB_LIBS) ../../bps/libbps.la

