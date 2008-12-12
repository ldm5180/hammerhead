SWIG_INCDIR = -I../libbionet -I../../util -I../../cal -I../../cal/util

lib_LTLIBRARIES = libbionet-py.la

libbionet_py_la_SOURCES = bionet_wrap.c

libbionet_py_la_CFLAGS = `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libbionet_py_la_LDFLAGS = -L../libbionet -L../../cal/mdnssd-bip/client -L../../asn.1 -L../../util
libbionet_py_la_LIBADD = -lbionet -lcal-mdnssd-bip-client -lbionet-asn -lbionet-util

bionet_wrap.c: bionet.i libbionet.la libcal-mdnssd-bip-client.la libbionet-asn.la libbionet-util.la
	swig -w302 -python ${SWIG_INCDIR} bionet.i 
	ln -sf .libs/libbionet-py.so _bionet.so

clean-local:
	rm -f bionet.py _bionet.so bionet_wrap.c libbionet*

EXTRA_DIST = bionet.py .libs/libbionet-py.so* libbionet-py.a