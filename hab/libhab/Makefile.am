include $(top_srcdir)/common.am

lib_LTLIBRARIES = libhab.la libhab-21.la

libhab_la_SOURCES =      \
    connect.c            \
    report-new-node.c    \
    report-lost-node.c   \
    report-datapoints.c  \
    read.c               \
    register-callback.c  \
    cal-callback.c       \
    cal-topic-matches.c  \
    global.c

libhab_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../../cal -I ../../cal/util -I../../util -I../../asn.1
libhab_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)


libhab_21_la_SOURCES =      \
    connect.c            \
    report-new-node.c    \
    report-lost-node.c   \
    report-datapoints.c  \
    read.c               \
    register-callback.c  \
    cal-callback.c       \
    cal-topic-matches.c  \
    global.c

libhab_21_la_CFLAGS = $(AM_CFLAGS) -DBIONET_21_API $(GLIB20_CFLAGS) -I../../cal -I ../../cal/util -I../../util -I../../asn.1
libhab_21_la_LIBADD = $(AM_LDADD) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(GLIB20_LIBS) $(DBUS_LIBS)

