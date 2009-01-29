include $(top_srcdir)/common.am

SWIG_INCDIR = $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_CAL_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT)

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

lib_LTLIBRARIES = libbionet-py.la

libbionet_py_la_SOURCES = bionet_wrap.c

libbionet_py_la_CFLAGS = `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libbionet_py_la_LDFLAGS = $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) -version-info $(BIONET_INTERFACE_VERSION)
libbionet_py_la_LIBADD = $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_UTIL) -lpthread -lm

libbionet_py_la_DEPENDENCIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)

bionet_wrap.c: bionet.i $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL)
	swig -w302 -w454 -python ${SWIG_INCDIR} bionet.i 
	ln -sf .libs/libbionet-py.so _bionet.so

install-exec-hook:
	python setup.py install
	cp _bionet.so $(LIB_INST_DIR)/.
	$(LDCONFIG_EXE)

clean-local:
	rm -f bionet.py _bionet.so bionet_wrap.c libbionet*

EXTRA_DIST = bionet.py .libs/libbionet-py.so* libbionet-py.a