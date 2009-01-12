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


speedway_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util -I LTK-1.0.0.5/LTKC/Library

speedway_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) -L../libhab -L../../util -L../../asn.1 -L../../cal/mdnssd-bip/server -L LTK-1.0.0.5/LTKC/Library/LLRP.org -L LTK-1.0.0.5/LTKC/Library

speedway_LDADD = $(AM_LDADD) -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lltkc -lltkcllrporg -lm -lpthread


# we need to compile the LLRP Tool Kit before compiling the speedway hab
BUILT_SOURCES = LTK
LTK_DIR = LTK-1.0.0.5
LTK_PATCH = ltkc-1.0.0.5.patch

$(LTK_DIR): ltkc_src_1_0_0_5.tgz
	tar -xzf ltkc_src_1_0_0_5.tgz
	mv LTK $(LTK_DIR)
	patch --dir=$(LTK_DIR) -p2 --quiet < $(LTK_PATCH) 

.PHONY: LTK
LTK: $(LTK_DIR)
	$(MAKE) -C LTK-1.0.0.5/LTKC

clean-local:
	rm -rf $(LTK_DIR)
