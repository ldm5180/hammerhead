include $(top_srcdir)/common.am

SWIG_INCDIR = -I../libhab -I../../util

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

lib_LTLIBRARIES = libhab-py.la

libhab_py_la_SOURCES = hab_wrap.c

libhab_py_la_CFLAGS = `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libhab_py_la_LDFLAGS = -L../libhab -L../../util -L../../asn.1 -L../../cal/mdnssd-bip/server -version-info $(BIONET_INTERFACE_VERSION)
libhab_py_la_LIBADD = -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server


hab_wrap.c: hab.i ../../util/bionet-util.h ../../util/bionet-hab.h ../../util/bionet-node.h ../../util/bionet-resource.h ../../util/bionet-datapoint.h ../../util/bionet-value.h ../libhab/hardware-abstractor.h
	swig -w302 -python ${SWIG_INCDIR} hab.i 
	ln -sf .libs/libhab-py.so _hab.so

clean-local:
	rm -f hab.py _hab.so hab_wrap.c libbionet*

install-exec-hook:
	python setup.py install
	cp _hab.so $(LIB_INST_DIR)/.
	$(LDCONFIG_EXE)

EXTRA_DIST = hab.py .libs/libhab-py.so* libhab-py.a