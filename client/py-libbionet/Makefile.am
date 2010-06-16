
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

SWIG_INCDIR = $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_CAL_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT)

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

python_PYTHON = bionet.py
python_LTLIBRARIES = _bionet.la

BUILT_SOURCES = bionet_wrap.c
_bionet_la_SOURCES = bionet_wrap.c

_bionet_la_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(SWIG_PYTHON_CPPFLAGS) $(SWIG_INCDIR) -fno-strict-aliasing
_bionet_la_LDFLAGS = -module -avoid-version $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL)
_bionet_la_LIBADD = $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_UTIL) -lpthread -lm

_bionet_la_DEPENDENCIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)

bionet_wrap.c: bionet.i $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)
	$(SWIG) -w302 -w454 $(SWIG_PYTHON_OPT) ${SWIG_INCDIR} -o $@ $<

clean-local:
	rm -f bionet.py _bionet.so bionet_wrap.c libbionet*
