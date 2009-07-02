
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

TESTS = check-asn1

check_PROGRAMS = check-asn1

nodist_check_asn1_SOURCES = \
	../check-common.c \
	check-asn1.c \
	check-asn1-coding.c

check_asn1_CFLAGS = $(AM_CFLAGS) $(CHECK_CFLAGS) -I.. $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1)  $(GLIB_CFLAGS)

check_asn1_LDFLAGS = $(CHECK_LIBS) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1)  $(GLIB_LIBS)

check_asn1_LDADD = $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1)

