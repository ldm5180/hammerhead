
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

SWIG_INCDIR = $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

lib_LTLIBRARIES = libhab-py.la

libhab_py_la_SOURCES = hab_wrap.c

libhab_py_la_CFLAGS = $(AM_CFLAGS) `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libhab_py_la_LDFLAGS = $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) -version-info $(BIONET_INTERFACE_VERSION)
libhab_py_la_LIBADD = $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER) -lpthread -lm
libhab_py_la_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 

hab_wrap.c: hab.i $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 
	swig -w302 -w454 -python ${SWIG_INCDIR} hab.i 
	ln -sf .libs/libhab-py.so _hab.so

clean-local:
	rm -f hab.py _hab.so hab_wrap.c libbionet*

install-exec-hook:
	python setup.py install
	cp _hab.so $(LIB_INST_DIR)/.
	$(LDCONFIG_EXE)

EXTRA_DIST = hab.py .libs/libhab-py.so* libhab-py.a
