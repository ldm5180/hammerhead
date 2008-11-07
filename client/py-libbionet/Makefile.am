SWIG_INCDIR = -I../libbionet -I../../util -I../../cal -I../../cal/util

lib_LTLIBRARIES = libbionet-py.la

libbionet_py_la_SOURCES = bionet_wrap.c

libbionet_py_la_CFLAGS = `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libbionet_py_la_LDFLAGS = -L../libbionet -L../../cal/mdnssd-bip -L../../asn.1 -L../../util
libbionet_py_la_LIBADD = -lbionet -lcal-mdnssd-bip-client -lbionet-asn -lbionet-util

bionet_wrap.c: bionet.i
	swig -python ${SWIG_INCDIR} bionet.i 
	ln -sf .libs/libbionet-py.so _bionet.so
