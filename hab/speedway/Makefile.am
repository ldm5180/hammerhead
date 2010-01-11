
# Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
# Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = speedway

speedway_SOURCES =         \
    main.c                 \
    usage.c                \
    make-reader-node.c     \
    read-from-bionet.c     \
    get-reader-config.c    \
    get-reader-capabilities.c    \
    poll-reader.c          \
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
    exception-event.c	   \
    handle-interrupt.c


speedway_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) -I LTK-1.0.0.5/LTKC/Library $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

speedway_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) -LLTK-1.0.0.5/LTKC/Library/LLRP.org -LLTK-1.0.0.5/LTKC/Library $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

speedway_LDADD = $(AM_LDADD) -lltkc -lltkcllrporg -lm -lpthread $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER)

speedway_DEPENDENCIES = LTK-1.0.0.5/LTKC/Library/LLRP.org/libltkcllrporg.a LTK-1.0.0.5/LTKC/Library/libltkc.a $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 

# we need to compile the LLRP Tool Kit before compiling the speedway hab
LTK_TARBALL = ltkc_src_1_0_0_5.tgz
LTK_DIR = LTK-1.0.0.5
LTK_PATCH = ltkc-1.0.0.5.patch
BUILT_SOURCES = $(LTK_DIR)

$(LTK_DIR):
	tar -xzf $(LTK_TARBALL)
	mv LTK $(LTK_DIR)
	patch --dir=$(LTK_DIR) -p1 --quiet < $(LTK_PATCH) 
	+$(MAKE) -C $(LTK_DIR)/LTKC

clean-local:
	rm -rf $(LTK_DIR)

