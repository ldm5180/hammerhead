include $(top_srcdir)/common.am

bin_PROGRAMS = pal-650

pal_650_SOURCES =             \
    pal-650-hab.c             \
    pal-connect.c             \
    pal-read.c                \
    pal-request-taglist.c     \
    cb-set-resource.c         \
    timeout-tags.c            \
    drop-node.c               \
    make-shutdowns-clean.c    \
    global.c

pal_650_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

pal_650_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

pal_650_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lm


