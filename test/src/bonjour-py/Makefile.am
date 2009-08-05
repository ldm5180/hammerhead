
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

#SWIG_INCDIR = $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_CAL_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT)

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

python_PYTHON = bonjour.py
python_LTLIBRARIES = _bonjour.la

BUILT_SOURCES = bonjour_wrap.c
_bonjour_la_SOURCES = bonjour_wrap.c

_bonjour_la_CFLAGS = $(AM_CFLAGS) $(SWIG_PYTHON_CPPFLAGS) $(SWIG_INCDIR) -fno-strict-aliasing
_bonjour_la_LDFLAGS = -module -version-info $(BIONET_INTERFACE_VERSION)
_bonjour_la_LIBADD = -ldns_sd $(LIBS_BIONET_ASN1) -lpthread -lm

#_bonjour_la_DEPENDENCIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)

bonjour_wrap.c: bonjour.i $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)
	$(SWIG) -w302 -w454 $(SWIG_PYTHON_OPT) $(SWIG_INCDIR) -o $@ $<

clean-local:
	$(LIBTOOL) --mode=clean rm -f bonjour.py bonjour.pyc _bonjour.so bonjour_wrap.c libbonjour*
