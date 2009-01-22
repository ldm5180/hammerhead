include $(top_srcdir)/common.am

lib_LTLIBRARIES = libamedevice.la

libamedevice_la_SOURCES =      \
	amedevice.c            \
	sum8.c      


libamedevice_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(BLUEZ_CFLAGS) -I../../cal -I ../../cal/util -I../../util -I../../asn.1
libamedevice_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS) $(BLUEZ_LIBS)


