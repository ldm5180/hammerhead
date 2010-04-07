
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = csa-cp-hab


csa_cp_hab_SOURCES =         \
    clear-csa.c              \
    csa-cp-hab.c             \
    download-session.c       \
    get-num-files.c          \
    make-shutdowns-clean.c   \
    ping.c                   \
    print-help.c             \
    probe.c                  \
    serial.c                 \
    set-resource.c


csa_cp_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

csa_cp_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_HAB) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

csa_cp_hab_LDADD = $(AM_LDADD) -lpthread -lm $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER)

csa_cp_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 
