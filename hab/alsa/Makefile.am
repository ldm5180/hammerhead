include $(top_srcdir)/common.am

bin_PROGRAMS = alsa-hab


alsa_hab_SOURCES =               \
    alsa-hab.c                   \
    cb-stream-data.c             \
    cb-stream-subscription.c     \
    cb-stream-unsubscription.c   \
    check-alsa-poll.c            \
    close-alsa-device.c          \
    disconnect-client.c          \
    discover-alsa-hardware.c     \
    read-producer-stream.c       \
    open-alsa-device.c           \
    show-state.c                 \
    xrun-handler.c

alsa_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_HAB) $(CFLAGS_BIONET_UTIL) 

alsa_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) $(LDFLAGS_BIONET_HAB) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_CAL_SERVER) 

alsa_hab_LDADD = $(AM_LDADD) -lasound $(LIBS_BIONET_HAB) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_SERVER) -lpthread -lm

alsa_hab_DEPENDENCIES = $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 

#install: all
#	$(INSTALL) -m 0755 alsa-hab               ${EXP_BIN_DIR}
#	$(INSTALL) -m 0755 init.d.alsa-hab        ${EXP_INIT_DIR}/alsa-hab
#	$(INSTALL) -m 0644 alsa-hab.conf          ${EXP_ETC_DIR}
#	$(INSTALL) -m 0755 audio-stream-producer  ${EXP_BIN_DIR}
#	$(INSTALL) -m 0755 audio-stream-consumer  ${EXP_BIN_DIR}
#	$(INSTALL) -m 0755 pipe.sh                ${EXP_BIN_DIR}
#	$(INSTALL) -m 0755 listen-to-all          ${EXP_BIN_DIR}
#	$(INSTALL) -m 0755 loopback.sh            ${EXP_BIN_DIR}
#	$(INSTALL) -m 0755 sanify-levels          ${EXP_BIN_DIR}

