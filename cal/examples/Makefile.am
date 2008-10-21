#AM_CFLAGS = -Wall -Werror -g -I.. -I../util `pkg-config --cflags glib-2.0`

bin_PROGRAMS = time-publisher time-subscriber

time_publisher_SOURCES = time-publisher.c
time_publisher_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(AVAHI_GLIB_CFLAGS) $(DBUS_CFLAGS) -I.. -I../util
time_publisher_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(AVAHI_GLIB_LIBS) $(DBUS_LIBS)
time_publisher_LDADD = $(AM_LDADD) ../mdnssd-bip/server/libcal-mdnssd-bip-server.la -lpthread 

time_subscriber_SOURCES = time-subscriber.c
time_subscriber_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_CFLAGS) $(AVAHI_GLIB_CFLAGS) $(DBUS_CFLAGS) -I.. -I../util
time_subscriber_LDFLAGS =  $(AM_LDFLAGS) $(DBUS_LIBS)
time_subscriber_LDADD = $(AM_LDADD) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(AVAHI_GLIB_LIBS) ../mdnssd-bip/client/libcal-mdnssd-bip-client.la -lpthread 


