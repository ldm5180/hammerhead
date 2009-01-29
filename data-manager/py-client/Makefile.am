include $(top_srcdir)/common.am

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

LIB_INST_DIR=`echo "import sys; print sys.prefix + \"/lib/python\" + str(sys.version_info[0]) + \".\" + str(sys.version_info[1])" | python`

lib_LTLIBRARIES = libbdm.la

libbdm_la_DEPENDENCIES = $(INCLUDES_BIONET_UTIL)

libbdm_la_SOURCES = bdm-client_wrap.c bdm-client-interface.c ../client/connect.c ../client/disconnect.c ../client/get-resource-datapoints.c ../client/send-asn.c  

libbdm_la_LIBADD = $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(GLIB20_LIBS)

libbdm_la_CFLAGS = $(CFLAGS_BIONET_UTIL) -I/usr/include/python2.5 $(CFLAGS_BIONET_ASN1) -I../include $(GLIB20_CFLAGS) -I../client

libbdm_la_LDFLAGS = $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1) -version-info $(BIONET_INTERFACE_VERSION) 

install-exec-hook: setup.py
	python setup.py install
	cp _bdm_client.so $(LIB_INST_DIR)/.
	$(LDCONFIG_EXE)

bdm-client_wrap.c: bdm-client.i bdm-client-interface.c $(INCLUDES_BIONET_UTIL)
	swig -python $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) -I../include -I../client bdm-client.i
	ln -sf .libs/libbdm.so _bdm_client.so

clean-local:
	rm -f *.so bdm-client_wrap.c bdm_client.*
