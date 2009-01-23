include $(top_srcdir)/common.am

SWIG_INCDIR = -I../libbionet -I../../util -I../../cal -I../../cal/util

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

lib_LTLIBRARIES = libbionet-py.la

libbionet_py_la_SOURCES = bionet_wrap.c

libbionet_py_la_CFLAGS = `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libbionet_py_la_LDFLAGS = -L../libbionet -L../../cal/mdnssd-bip/client -L../../asn.1 -L../../util -version-info $(BIONET_INTERFACE_VERSION)
libbionet_py_la_LIBADD = -lbionet -lcal-mdnssd-bip-client -lbionet-asn -lbionet-util


bionet_wrap.c: bionet.i ../libbionet/libbionet.la ../../cal/mdnssd-bip/client/libcal-mdnssd-bip-client.la ../../asn.1/libbionet-asn.la ../../util/libbionet-util.la
	swig -w302 -python ${SWIG_INCDIR} bionet.i 
	ln -sf .libs/libbionet-py.so _bionet.so

install-exec-hook:
	python setup.py install
	cp _bionet.so $(LIB_INST_DIR)/.
	$(LDCONFIG_EXE)

clean-local:
	rm -f bionet.py _bionet.so bionet_wrap.c libbionet*

EXTRA_DIST = bionet.py .libs/libbionet-py.so* libbionet-py.a