
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = stethoscope-hab


stethoscope_hab_SOURCES =          		\
	cb-stream-data.c			\
	cb-stream-subscription.c		\
	cb-stream-unsubscription.c		\
	daemonize.c        			\
	main.c             			\
	mulaw2pcm16.c      			\
	pidfile.c          			\
	read_data_from_stethoscope_and_write.c  \
	set_resource.c     			


stethoscope_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(BLUEZ_CFLAGS) $(CFLAGS_BIONET_HAB_AME) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

stethoscope_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(BLUEZ_LIBS) $(LDFLAGS_BIONET_HAB_AME) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

stethoscope_hab_LDADD = $(AM_LDADD) -lpthread -lm $(LIBS_BIONET_HAB_AME) $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER)

stethoscope_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_HAB_AME)
