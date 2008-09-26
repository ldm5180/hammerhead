
include ../../Makefile.params

CFLAGS += -I .. -I ../..

LDLIBS = `pkg-config --libs glib-2.0`

ifeq (${TARGET_OS},LINUX)
CFLAGS += `pkg-config --cflags avahi-compat-libdns_sd`
LDLIBS += `pkg-config --libs avahi-compat-libdns_sd`
endif

OBJS =             \
    cal-pd-api.o   \
    register.o     \
    browse.o       \
    global.o       \

COMMON_OBJS =                              \
    ../shared/free-event.o                 \
    ../shared/free-peer.o                  \
    ../shared/peer-add-unicast-address.o   \


all: cal-pd.a dnssd-announcer

cal-pd.a: cal-pd.a($(OBJS) $(COMMON_OBJS))

dnssd-announcer: dnssd-announcer.o cal-pd.a

clean:
	rm -f dnssd-announcer dnssd-announcer.o cal-pd.a $(OBJS)

test:

install: cal-pd.a
	install -m 0644 cal-pd.a $(DESTDIR)/usr/lib/

.PHONY: all test install clean

