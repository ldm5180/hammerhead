
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

python_PYTHON = hab.py
python_LTLIBRARIES = _hab.la

BUILT_SOURCES = hab_wrap.c
_hab_la_SOURCES = hab_wrap.c

_hab_la_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(SWIG_PYTHON_CPPFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) -fno-strict-aliasing -DLIBHAB
_hab_la_LDFLAGS = -module -avoid-version $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER)
_hab_la_LIBADD = $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER) -lpthread -lm
_hab_la_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 

hab_wrap.c: hab.i $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) $(INCLUDES_SWIG_BIONET_UTIL)
	$(SWIG) $(SWIG_PYTHON_OPT) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) -DLIBHAB -o $@ $< 

clean-local:
	rm -f hab.py _hab.so hab_wrap.c libbionet*
