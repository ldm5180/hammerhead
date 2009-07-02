
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = cpod-hab


cpod_hab_SOURCES = 	   \
	available_opcodes.c\
	create_zeroed_float_sensor.c\
	crcccitt.c         \
	daemonize.c        \
	main.c             \
	pidfile.c          \
	read_next_packet.c \
	reliable_io.c      \
	sampling_parameters.c\
	send_next_packet_streaming.c\
	set_time.c         \
	set_float_resource.c\
	shake_hands.c      \
	start_streaming.c  \
	next_command_number.c  \
	verify_response.c

cpod_hab_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(BLUEZ_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

cpod_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) $(BLUEZ_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_HAB) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

cpod_hab_LDADD = $(AM_LDADD) $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER) -lpthread -lm

cpod_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 
