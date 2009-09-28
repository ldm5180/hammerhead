
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-data-manager
noinst_LTLIBRARIES = libbionet-data-manager.la

AM_YFLAGS = -d 
AM_LFLAGS = --yylineno

BUILT_SOURCES = cfg-parser.h

bionet_data_manager_SOURCES =         \
    bionet-data-manager.c             

libbionet_data_manager_la_SOURCES =      \
    bionet-io.c                       \
    db.c                              \
    client-connecting-handler.c       \
    client-readable-handler.c         \
    handle-client-message.c           \
    disconnect-client.c               \
    make-listening-socket.c           \
    make-shutdowns-clean.c            \
    keepalive.c                       \
    sync-send.c                       \
    sync-receive-connecting-handler.c \
    sync-receive-readable-handler.c   \
    sync-receive-ion.c                \
    handle-sync-datapoints-message.c  \
    handle-sync-metadata-message.c    \
    cfg-parser.y                      \
    cfg-lexer.l

bionet_data_manager_CFLAGS = $(AM_CFLAGS) $(OPENSSL_CFLAGS) $(GLIB_CFLAGS) $(OPENSSL_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) $(ION_CFLAGS)

bionet_data_manager_LDFLAGS = $(AM_LDFLAGS) $(OPENSSL_LIBS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(ION_LDFLAGS)

bionet_data_manager_LDADD = libbionet-data-manager.la  $(LIBS_ION) $(AM_LDADD) -lpthread -lm 

bionet_data_manager_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1) $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_CAL_CLIENT) $(BUILT_SOURCES) $(libbionet_data_manager_la_SOURCES) $(noinst_LTLIBRARIES)


libbionet_data_manager_la_CFLAGS = $(AM_CFLAGS) $(OPENSSL_CFLAGS) $(GLIB_CFLAGS) $(OPENSSL_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) $(ION_CFLAGS) -fno-strict-aliasing

libbionet_data_manager_la_LDFLAGS = $(AM_LDFLAGS) $(OPENSSL_LIBS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(ION_LDFALGS)

libbionet_data_manager_la_LIBADD = $(AM_LDADD) -lsqlite3 $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_ION) -lm 

libbionet_data_manager_la_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1) $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_CAL_CLIENT) 

clean-local:
	rm -f  cfg-parser.h cfg-parser.c cfg-lexer.c
