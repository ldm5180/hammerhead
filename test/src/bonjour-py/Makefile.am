
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

#SWIG_INCDIR = $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_CAL_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT)

noinst_PYTHON = bonjour.py
noinst_LTLIBRARIES = _bonjour.la

BUILT_SOURCES = bonjour_wrap.c
_bonjour_la_SOURCES = bonjour_wrap.c

_bonjour_la_CFLAGS = $(AM_CFLAGS) $(SWIG_PYTHON_CPPFLAGS) $(SWIG_INCDIR) -fno-strict-aliasing
_bonjour_la_LDFLAGS = -module 
_bonjour_la_LIBADD = $(AVAHI_COMPAT_LIBDNS_SD_LIBS) -lpthread -lm

#_bonjour_la_DEPENDENCIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)

bonjour_wrap.c: bonjour.i $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)
	$(SWIG) -w302 -w454 $(SWIG_PYTHON_OPT) $(SWIG_INCDIR) -o $@ $<

clean-local:
	$(LIBTOOL) --mode=clean rm -f bonjour.py bonjour.pyc _bonjour.so bonjour_wrap.c libbonjour*
