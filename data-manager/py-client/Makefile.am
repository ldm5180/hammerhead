
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

python_PYTHON = bdm_client.py
python_LTLIBRARIES = _bdm_client.la

_bdm_client_la_DEPENDENCIES = $(INCLUDES_BIONET_UTIL)

BUILT_SOURCES = bdm-client_wrap.c
_bdm_client_la_SOURCES = bdm-client_wrap.c bdm-client-interface.c ../client/connect.c ../client/disconnect.c ../client/get-resource-datapoints.c ../client/send-asn.c  
#SWIG_SOURCES = bdm-client.i

_bdm_client_la_LIBADD = $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(GLIB20_LIBS)

_bdm_client_la_CFLAGS = $(AM_CFLAGS) $(CFLAGS_BIONET_UTIL) $(SWIG_PYTHON_CPPFLAGS) $(CFLAGS_BIONET_ASN1) -I../include $(GLIB20_CFLAGS) -I../client

_bdm_client_la_LDFLAGS = -module $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1) -version-info $(BIONET_INTERFACE_VERSION) 


bdm-client_wrap.c: bdm-client.i bdm-client-interface.c $(INCLUDES_BIONET_UTIL)
	$(SWIG) $(SWIG_PYTHON_OPT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) -I../include -I../client -o $@ $<

clean-local:
	rm -f *.so bdm-client_wrap.c bdm_client.*
