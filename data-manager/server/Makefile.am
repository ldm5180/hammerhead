
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-data-manager


bionet_data_manager_SOURCES =         \
    bionet-data-manager.c             \
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
    handle-sync-datapoints-message.c  \
    handle-sync-metadata-message.c    

bionet_data_manager_CFLAGS = $(AM_CFLAGS) $(OPENSSL_CFLAGS) $(GLIB20_CFLAGS) $(OPENSSL_CFLAGS) -I../include $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_ASN1) 

bionet_data_manager_LDFLAGS = $(AM_LDFLAGS) $(OPENSSL_LIBS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT)

bionet_data_manager_LDADD = $(AM_LDADD) -lsqlite3 $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT) -lpthread -lm

bionet_data_manager_DEPENDENCIES = $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1) $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_CAL_CLIENT) 
