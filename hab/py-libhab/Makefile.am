SWIG_INCDIR = -I../libhab -I../../util

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

lib_LTLIBRARIES = libhab-py.la

libhab_py_la_SOURCES = hab_wrap.c

libhab_py_la_CFLAGS = -DBIONET_21_API `pkg-config --cflags glib-2.0` -I/usr/include/python2.5 ${SWIG_INCDIR}
libhab_py_la_LDFLAGS = -L../libhab -L../../util -L../../asn.1 -L../../cal/mdnssd-bip/server
libhab_py_la_LIBADD = -lhab-21 -lbionet-util-21 -lbionet-asn-21 -lcal-mdnssd-bip-server

hab_wrap.c: hab.i
	swig -DBIONET_21_API -w302 -python ${SWIG_INCDIR} hab.i 
	ln -sf .libs/libhab-py.so _hab.so

clean-local:
	rm -f hab.py _hab.so hab_wrap.c libbionet*

install-exec-hook:
	python setup.py install
	cp _hab.so $(LIB_INST_DIR)/.
	$(LDCONFIG_EXE)

EXTRA_DIST = hab.py .libs/libhab-py.so* libhab-py.a