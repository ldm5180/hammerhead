include $(top_srcdir)/common.am

bin_PROGRAMS = alsa-hab


alsa_hab_SOURCES =               \
    alsa-hab.c                   \
    check-alsa-poll.c            \
    close-alsa-device.c          \
    connect-client.c             \
    disconnect-client.c          \
    discover-alsa-hardware.c     \
    handle-client.c              \
    open-alsa-device.c           \
    show-state.c                 \
    xrun-handler.c

alsa_hab_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libhab -I../../util

alsa_hab_LDFLAGS = $(AM_LDFLAGS) $(GLIB20_LIBS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(DBUS_LIBS) -L../libhab -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/server

alsa_hab_LDADD = $(AM_LDADD) -lasound -lhab -lbionet-util -lbionet-asn -lcal-mdnssd-bip-server -lpthread -lm


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
