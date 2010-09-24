
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.

#
# Set The SDL Version.
#
# For detailed explanation of this versioning scheme, 
# See: http://sourceware.org/autobook/autobook/autobook_91.html#SEC91
#
# INTERFACE:REVISION:AGE
#
# Interface is bumped on API breaking events
# Revision is bumped on each change like a bug fix
# Age is bumped on every addition to the API
BPS_INTERFACE_VERSION=0:0:0

#Purely aesthetic marketing version number
BPS_RELEASE_VERSION=0.0.0

include $(top_srcdir)/common.am

SUBDIRS = . utils check

lib_LTLIBRARIES = libbps.la

include_HEADERS = \
    bps_socket.h

libbps_la_SOURCES =    \
    bps-ion.c \
    bps-sock-table.c \
    load-ion.c \
    ion-recv-thread.c \
    ion-send-thread.c 

libbps_la_CFLAGS = $(AM_CFLAGS) -pthread $(ION_CFLAGS)

libbps_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(BPS_INTERFACE_VERSION) \
    -no-undefined \
    -pthread

libbps_la_LIBADD = -ldl
