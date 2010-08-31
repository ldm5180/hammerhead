include $(top_srcdir)/common.am

bin_PROGRAMS = translator-clab
translator_clab_SOURCES = main.c cb-datapoint.c calibration_tables.c signal_handler.c usage.c create-node.c create-resource.c cb-set-resource.c find-voltage.c ini-read.c
translator_clab_CFLAGS = -g $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) -std=c99
translator_clab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER)
translator_clab_LDADD = $(AM_LDADD) $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) -lm $(LIBS_BIONET_CLIENT)
translator_clab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT)
