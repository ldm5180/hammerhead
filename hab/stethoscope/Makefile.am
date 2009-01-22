include $(top_srcdir)/common.am

bin_PROGRAMS = stethoscope-hab


stethoscope_hab_SOURCES =          		\
	daemonize.c        			\
	main.c             			\
	mulaw2pcm16.c      			\
	pidfile.c          			\
	read_data_from_stethoscope_and_write.c  \
	set_resource.c     			


stethoscope_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(BLUEZ_CFLAGS) -I../libhab -I../../util -I../libamedevice

stethoscope_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(BLUEZ_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server -L../libamedevice

stethoscope_hab_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lamedevice -lpthread -lm

