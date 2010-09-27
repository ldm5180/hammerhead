
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

python_PYTHON = bdm_client.py
python_LTLIBRARIES = _bdm_client.la

#_bdm_client_la_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) 

BUILT_SOURCES = bdm-client_wrap.c
_bdm_client_la_SOURCES = bdm-client_wrap.c bdm-client-interface.c  

_bdm_client_la_LIBADD = $(LIBS_BDM) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(GLIB_LIBS)

_bdm_client_la_CFLAGS = $(AM_CFLAGS) $(CFLAGS_BDM) $(SWIG_PYTHON_CPPFLAGS) -fno-strict-aliasing

_bdm_client_la_LDFLAGS = -module -avoid-version $(LDFLAGS_BDM) $(LDFLAGS_BIONET_ASN1)


bdm-client_wrap.c: bdm-client.i bdm-client-interface.c $(INCLUDES_BIONET_UTIL) $(INCLUDES_BDM) $(INCLUDES_BIONET_ASN1) $(INCLUDES_SWIG_BIONET_UTIL)
	$(SWIG) -w302 -w454 $(SWIG_PYTHON_OPT) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) -I../../../libbdm -o $@ $<

clean-local:
	rm -f *.so bdm-client_wrap.c bdm_client.*
