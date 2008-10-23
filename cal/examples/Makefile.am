include $(top_srcdir)/common.am

bin_PROGRAMS = time-publisher time-subscriber


time_publisher_SOURCES = time-publisher.c

time_publisher_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(DBUS_CFLAGS) -I.. -I../util

time_publisher_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../mdnssd-bip/server

time_publisher_LDADD = $(AM_LDADD) -lcal-mdnssd-bip-server -lpthread 



time_subscriber_SOURCES = time-subscriber.c

time_subscriber_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(DBUS_CFLAGS) -I.. -I../util

time_subscriber_LDFLAGS =  $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../mdnssd-bip/client

time_subscriber_LDADD = $(AM_LDADD)  -lcal-mdnssd-bip-client -lpthread 


