
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-data-manager
noinst_LTLIBRARIES = libbionet-data-manager.la

AM_CFLAGS += -fstrict-aliasing -Wstrict-aliasing
AM_YFLAGS = -d 
AM_LFLAGS = --yylineno

BUILT_SOURCES = cfg-parser.h

bionet_data_manager_SOURCES =         \
    bionet-data-manager.c             

libbionet_data_manager_la_SOURCES =      \
    bionet-io.c                       \
    db.c                              \
    db-batch.c                        \
    bdm-asn.c                         \
    cal-callback.c                    \
    cal-topic-matches.c               \
    client-connecting-handler.c       \
    client-readable-handler.c         \
    handle-client-message.c           \
    disconnect-client.c               \
    make-listening-socket.c           \
    make-shutdowns-clean.c            \
    keepalive.c                       \
    subscription.c                    \
    sync-send.c                       \
    sync-receive-connecting-handler.c \
    sync-receive-readable-handler.c   \
    sync-receive-ion.c                \
    sync-record-asn.c                 \
    publish.c                         \
    handle-sync-datapoints-message.c  \
    handle-sync-metadata-message.c    \
    cfg-parser.y                      \
    cfg-lexer.l

bin_SCRIPTS = bdm-make-db
bionetdata_DATA = schema

nobase_sysconf_DATA =           \
    init.d/bionet-data-manager  \
    bdm.ini.default


extra_cflags = $(OPENSSL_CFLAGS) $(GLIB_CFLAGS) $(OPENSSL_CFLAGS) $(CFLAGS_BIONET_CAL_SERVER) $(CFLAGS_BIONET_CAL_UTIL) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) $(ION_CFLAGS) $(CFLAGS_BIONET_HAB)

bionet_data_manager_CFLAGS = $(AM_CFLAGS) $(extra_cflags)

bionet_data_manager_LDFLAGS = $(AM_LDFLAGS) $(OPENSSL_LIBS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_HAB)

bionet_data_manager_LDADD = libbionet-data-manager.la  $(AM_LDADD) -lpthread -lm $(LIBS_BIONET_CAL_SERVER) $(LIBS_BIONET_HAB) -ldl

bionet_data_manager_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1) $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_CAL_CLIENT) $(BUILT_SOURCES) $(libbionet_data_manager_la_SOURCES) $(noinst_LTLIBRARIES) $(INCLUDES_BIONET_HAB)


libbionet_data_manager_la_CFLAGS = $(AM_CFLAGS) $(extra_cflags) -fno-strict-aliasing

libbionet_data_manager_la_LDFLAGS = $(AM_LDFLAGS) $(OPENSSL_LIBS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT)

libbionet_data_manager_la_LIBADD = $(AM_LDADD) -lsqlite3 $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CAL_SERVER) -lm 

libbionet_data_manager_la_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1) $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_CAL_CLIENT) 

clean-local:
	rm -f  cfg-parser.h cfg-parser.c cfg-lexer.c
