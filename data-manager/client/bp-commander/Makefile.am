
# Copyright (c) 2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS =                \
    bionet-bp-commander       \
    bionet-bp-command-proxy


bionet_bp_commander_SOURCES = bionet-bp-commander.c

bionet_bp_commander_CFLAGS = \
        $(CFLAGS_BDM) \
        $(ION_CFLAGS) \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CFLAGS_BIONET_CLIENT) \
	$(CFLAGS_BIONET_UTIL) \
	$(CFLAGS_BIONET_CAL_UTIL) \
	$(CFLAGS_BIONET_CAL_CLIENT)

bionet_bp_commander_LDFLAGS = \
	$(LDFLAGS_BDM) \
	$(ION_LDFLAGS) \
	$(AM_LDFLAGS) \
	$(GLIB_LIBS) \
	$(AVAHI_COMPAT_LIBDNS_SD_LIBS) \
	$(DBUS_LIBS) \
	$(LDFLAGS_BIONET_CLIENT) \
	$(LDFLAGS_BIONET_ASN1) \
	$(LDFLAGS_BIONET_UTIL) \
	$(LDFLAGS_BIONET_CAL_UTIL) \
	$(LDFLAGS_BIONET_CAL_CLIENT) 

bionet_bp_commander_LDADD = \
	$(LIBS_BDM) \
	$(LIBS_ION) \
	$(LIBS_BIONET_CAL_UTIL) \
	$(LIBS_BIONET_CAL_CLIENT) \
	$(OPENSSL_LIBS) \
	$(LIBS_BIONET_UTIL) \
	$(LIBS_BIONET_ASN1) \
	$(LIBS_BIONET_CLIENT) \
	-lpthread \
	-lm 


bionet_bp_command_proxy_SOURCES = bionet-bp-command-proxy.c

bionet_bp_command_proxy_CFLAGS = \
        $(ION_CFLAGS) \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CFLAGS_BIONET_CLIENT) \
	$(CFLAGS_BIONET_UTIL) \
	$(CFLAGS_BIONET_CAL_UTIL) \
	$(CFLAGS_BIONET_CAL_CLIENT)

bionet_bp_command_proxy_LDFLAGS = \
	$(ION_LDFLAGS) \
	$(AM_LDFLAGS) \
	$(GLIB_LIBS) \
	$(AVAHI_COMPAT_LIBDNS_SD_LIBS) \
	$(DBUS_LIBS) \
	$(LDFLAGS_BIONET_CLIENT) \
	$(LDFLAGS_BIONET_ASN1) \
	$(LDFLAGS_BIONET_UTIL) \
	$(LDFLAGS_BIONET_CAL_UTIL) \
	$(LDFLAGS_BIONET_CAL_CLIENT) 

bionet_bp_command_proxy_LDADD = \
	$(LIBS_ION) \
	$(LIBS_BIONET_CAL_UTIL) \
	$(LIBS_BIONET_CAL_CLIENT) \
	$(OPENSSL_LIBS) \
	$(LIBS_BIONET_UTIL) \
	$(LIBS_BIONET_ASN1) \
	$(LIBS_BIONET_CLIENT) \
	-lpthread \
	-lm 

