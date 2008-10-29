include $(top_srcdir)/common.am

lib_LTLIBRARIES = libbionet.la

libbionet_la_SOURCES = \
    set-id.c           \
    global.c

libbionet_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../../cal -I../../util
libbionet_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)

