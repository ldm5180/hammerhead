include $(top_srcdir)/common.am

bin_PROGRAMS = cgba-sim-proxr-hab
cgba_sim_proxr_hab_SOURCES = main.c proxrcmds.c proxrport.c cb-set-resource.c create-resource.c create-node.c usage.c signal-handler.c sim-hab.h
cgba_sim_proxr_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) -std=c99
cgba_sim_proxr_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER)
cgba_sim_proxr_hab_LDADD = $(AM_LDADD) $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) -lm
cgba_sim_proxr_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL)
