
include ../../Makefile.params

CFLAGS += -I .. -I ../..

COMMON_OBJS =               \
    ../shared/free-event.o  \
    ../shared/free-peer.o   \


OBJS =                      \
    cal-pd-api.o            \
    announcer.o             \
    browser.o               \
    generate-leave-event.o  \
    generate-join-event.o   \
    send-peer-info-ack.o    \
    hexdump.o               \
    make-mcast-socket.o     \
    make-unicast-socket.o   \
    mcast-message.o         \
    unicast-message.o       \
    global.o                \
    tv-utils.o              \


all: cal-pd.a

cal-pd.a: cal-pd.a($(OBJS) $(COMMON_OBJS))

clean:
	rm -f $(OBJS) cal-pd.a

install:

test:

.PHONY: all test install clean

