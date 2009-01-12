include $(top_srcdir)/common.am

bin_PROGRAMS = speedway

speedway_SOURCES =         \
    main.c                 \
    add-node.c             \
    access-report.c        \
    connect.c              \
    configure.c            \
    global.c               \
    start-spec.c           \
    recv-message.c         \
    scrub-config.c         \
    free-message.c         \
    handle-event.c         \
    tag-report.c           \
    conn-status.c          \
    add-spec.c             \
    enable-spec.c          \
    transact.c             \
    llrp-status.c          \
    xml-message.c          \
    factory-defaults.c     \
    delete-specs.c         \
    antenna-event.c        \
    exception-event.c      \
    one-tag-report.c


speedway_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util -I../../../Impinj/LTK/LTKC/Library

speedway_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) -L../libhab -L../../util -L../../asn.1 -L../../cal/mdnssd-bip/server -L../../../Impinj/LTK/LTKC/Library/LLRP.org -L../../../Impinj/LTK/LTKC/Library

speedway_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lltkc -lltkcllrporg -lm -lpthread


