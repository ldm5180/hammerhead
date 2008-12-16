include $(top_srcdir)/common.am

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

lib_LTLIBRARIES = libbdm.la

libbdm_la_DEPENDENCIES = ../client/bdm-client

libbdm_la_SOURCES = bdm-client_wrap.c bdm-client-interface.c ../client/connect.c ../client/disconnect.c ../client/get-resource-datapoints.c ../client/send-asn.c  

libbdm_la_LIBADD = -lbionet-util -lbionet-asn $(GLIB20_LIBS)

libbdm_la_CFLAGS = -I../../util -I/usr/include/python2.5 -I../../asn.1 -I../include $(GLIB20_CFLAGS) -L../../util -L../../asn.1 -I../client

install-exec-hook: setup.py
	python setup.py install
	cp _bdm_client.so $(LIB_INST_DIR)/.
	$(LDCONFIG_EXE)

bdm-client_wrap.c: bdm-client.i bdm-client-interface.c
	swig -python -I../../util -I../../asn.1 -I../include -I../client bdm-client.i
	ln -sf .libs/libbdm.so _bdm_client.so

clean-local:
	rm -f *.so bdm-client_wrap.c bdm_client.*
