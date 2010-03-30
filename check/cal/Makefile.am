
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

TESTS = check-cal

check_PROGRAMS = check-cal

nodist_check_cal_SOURCES = \
	../check-common.c \
	check-itc.c \
	check-cal.c

check_cal_CFLAGS = $(AM_CFLAGS) $(CHECK_CFLAGS) -I.. $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_CAL_CLIENT)  $(GLIB_CFLAGS) -fno-strict-aliasing

check_cal_LDFLAGS = $(CHECK_LIBS) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_CAL_UTIL)  $(GLIB_LIBS)

check_cal_LDADD = $(LIBS_BIONET_UTIL) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CAL_UTIL) $(CHECK_LIBS)

