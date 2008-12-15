include $(top_srcdir)/common.am

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

bin_PROGRAMS = bdm-client

bdm_client_SOURCES =            \
    bdm-client.c                \
    connect.c                   \
    disconnect.c                \
    get-resource-datapoints.c   \
    send-asn.c

bdm_client_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../include -I../../util -I../../asn.1

bdm_client_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../../asn.1 -L../../util

bdm_client_LDADD = $(AM_LDADD) -lbionet-util -lbionet-asn -lpthread -lm


if ENABLE_PYTHON

lib_LTLIBRARIES = libbdm.la

libbdm_la_DEPENDENCIES = bdm-client

libbdm_la_SOURCES = bdm-client_wrap.c bdm-client-interface.c connect.c disconnect.c get-resource-datapoints.c send-asn.c  

libbdm_la_LIBADD = -lbionet-util -lbionet-asn $(GLIB20_LIBS)

libbdm_la_CFLAGS = -I../../util -I/usr/include/python2.5 -I../../asn.1 -I../include $(GLIB20_CFLAGS) -L../../util -L../../asn.1

install-exec-hook:
	ln -sf $(exec_prefix)/lib/libbdm.so $(exec_prefix)/lib/_bdm_client.so
	$(LDCONFIG_EXE)
	python setup.py install

bdm-client_wrap.c: bdm-client.i bdm-client-interface.c
	swig -python -I../../util -I../../asn.1 -I../include bdm-client.i
	ln -sf .libs/libbdm.so _bdm_client.so

clean-local:
	rm -f *.so bdm-client_wrap.c

endif
