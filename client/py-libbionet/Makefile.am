SWIG_INCDIR = -I../libbionet -I../../util

lib_LTLIBRARIES = libbionet-py.la

libbionet_py_la_SOURCES = bionet_wrap.c

libbionet_py_la_CFLAGS = `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 -I. -I../../asn.1 -I../../util -I../libbionet
libbionet_py_la_LDFLAGS = -L../libbionet
libbionet_py_la_LIBADD = -lbionet

bionet_wrap.c: bionet.i
	swig -python ${SWIG_INCDIR} bionet.i 
	ln -s .libs/libbionet-py.so _libbionet-py.so
