include $(top_srcdir)/common.am

bin_PROGRAMS = speedway

speedway_SOURCES =         \
    main.c                 \
    make-reader-node.c     \
    get-reader-config.c    \
    poll-for-report.c      \
    handle-tag-report-data.c  \
    connect.c              \
    configure.c            \
    configure-reader.c     \
    set-gpo.c              \
    read-gpis.c            \
    global.c               \
    recv-message.c         \
    scrub-config.c         \
    free-message.c         \
    handle-event.c         \
    handle-gpi-event.c     \
    conn-status.c          \
    add-spec.c             \
    start-spec.c           \
    enable-spec.c          \
    transact.c             \
    llrp-status.c          \
    xml-message.c          \
    factory-defaults.c     \
    delete-specs.c         \
    antenna-event.c        \
    exception-event.c


speedway_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util -I LTK-1.0.0.5/LTKC/Library

speedway_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) -L../libhab -L../../util -L../../asn.1 -L../../cal/mdnssd-bip/server -LLTK-1.0.0.5/LTKC/Library/LLRP.org -LLTK-1.0.0.5/LTKC/Library

speedway_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lltkc -lltkcllrporg -lm -lpthread


# we need to compile the LLRP Tool Kit before compiling the speedway hab
LTK_TARBALL = ltkc_src_1_0_0_5.tgz
LTK_DIR = LTK-1.0.0.5
LTK_PATCH = ltkc-1.0.0.5.patch
BUILT_SOURCES = $(LTK_DIR)

$(LTK_DIR):
	tar -xzf $(LTK_TARBALL)
	mv LTK $(LTK_DIR)
	patch --dir=$(LTK_DIR) -p2 --quiet < $(LTK_PATCH) 
	$(MAKE) -C $(LTK_DIR)/LTKC

clean-local:
	rm -rf $(LTK_DIR)
