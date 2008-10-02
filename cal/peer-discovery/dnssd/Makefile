
include ../../Makefile.params

CFLAGS += -I ../.. -I ../../util

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

COMMON_OBJS = $(wildcard ../../util/*.o)


all: cal-pd.a

cal-pd.a: cal-pd.a($(OBJS) $(COMMON_OBJS))

clean:
	rm -f cal-pd.a $(OBJS)

test:

install: cal-pd.a
	install -m 0644 cal-pd.a $(DESTDIR)/usr/lib/

.PHONY: all test install clean

