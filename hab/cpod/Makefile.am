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

cpod_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(BLUEZ_CFLAGS) -I../libhab -I../../util -I../libamedevice

cpod_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(BLUEZ_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server -L../libamedevice

cpod_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm -lamedevice
