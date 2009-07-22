
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-get


bionet_get_SOURCES = bionet-get.c

bionet_get_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(OPENSSL_CFLAGS) \
	$(CFLAGS_BIONET_CLIENT) \
	$(CFLAGS_BIONET_UTIL) \
	$(CFLAGS_BIONET_CAL_UTIL) \
	$(CFLAGS_BIONET_CAL_CLIENT)

bionet_get_LDFLAGS = \
	$(AM_LDFLAGS) \
	$(GLIB_LIBS) \
	$(AVAHI_COMPAT_LIBDNS_SD_LIBS) \
	$(DBUS_LIBS) \
	$(LDFLAGS_BIONET_CLIENT) \
	$(LDFLAGS_BIONET_ASN1) \
	$(LDFLAGS_BIONET_UTIL) \
	$(LDFLAGS_BIONET_CAL_UTIL) \
	$(LDFLAGS_BIONET_CAL_CLIENT) 

bionet_get_LDADD = \
	$(AM_LDADD) \
	$(LIBS_BIONET_CAL_UTIL) \
	$(LIBS_BIONET_CAL_CLIENT) \
	$(OPENSSL_LIBS) \
	$(LIBS_BIONET_UTIL) \
	$(LIBS_BIONET_ASN1) \
	$(LIBS_BIONET_CLIENT) \
	-lpthread \
	-lm 

