
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = time-publisher time-subscriber

time_publisher_SOURCES = time-publisher.c

time_publisher_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	-I.. \
	-I../util \
	$(OPENSSL_CFLAGS) \
	$(CFLAGS_EMBEDDED_AVAHI)

time_publisher_LDFLAGS = \
	$(AM_LDFLAGS) \
	-L../mdnssd-bip/server \
	$(LDFLAGS_EMBEDDED_AVAHI)

time_publisher_LDADD = \
	$(AM_LDADD) \
	$(OPENSSL_LIBS) \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(LIBS_EMBEDDED_AVAHI) \
	-lcal-mdnssd-bip-server 


time_subscriber_SOURCES = time-subscriber.c

time_subscriber_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(DBUS_CFLAGS) \
	-I.. \
	-I../util \
	$(OPENSSL_CFLAGS) \
	$(CFLAGS_EMBEDDED_AVAHI)

time_subscriber_LDFLAGS = \
	$(AM_LDFLAGS) \
	-L../mdnssd-bip/client \
	$(LDFLAGS_EMBEDDED_AVAHI)

time_subscriber_LDADD = \
	$(AM_LDADD)  \
	-lcal-mdnssd-bip-client \
	$(OPENSSL_LIBS) \
	$(GLIB_LIBS) \
	$(LIBS_EMBEDDED_AVAHI) \
	$(DBUS_LIBS)


