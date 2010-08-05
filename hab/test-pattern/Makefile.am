
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

AM_YFLAGS = -d

BUILT_SOURCES = parser.h

bin_PROGRAMS = test-pattern-hab

test_pattern_hab_SOURCES =      \
    parser.y                    \
    lexer.l                     \
    create-resource.c           \
    new-event.c                 \
    free-event.c                \
    output.c                    \
    simulate-update.c           \
    str-to-timeval.c            \
    str-to-value.c              \
    test-pattern-hab.c

test_pattern_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_HAB)  $(CFLAGS_BIONET_UTIL)

test_pattern_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_SERVER) $(LDFLAGS_BOUNET_ASN1)

test_pattern_hab_LDADD = $(AM_LDADD) $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_CAL_SERVER) $(LIBS_BIONET_ASN1) -lpthread -lm

test_pattern_hab_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) $(INCLUDE_BIONET_HAB)

clean-local:
	rm -f parser.c parser.h lexer.c
