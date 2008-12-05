SWIG_INCDIR = -I../libhab -I../../util

lib_LTLIBRARIES = libhab-py.la

libhab_py_la_SOURCES = hab_wrap.c

libhab_py_la_CFLAGS = `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libhab_py_la_LDFLAGS = -L../libhab -L../../util -L../../asn.1 -L../../cal/mdnssd-bip/server
libhab_py_la_LIBADD = -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server

hab_wrap.c: hab.i
	swig -w302 -python ${SWIG_INCDIR} hab.i 
	ln -sf .libs/libhab-py.so _hab.so

clean-local:
	rm -f hab.py _hab.so hab_wrap.c libbionet*

EXTRA_DIST = hab.py .libs/libhab-py.so* libhab-py.a