
include ../../Makefile.params

CFLAGS += -I .. -I ../.. -I ../../peer-discovery

COMMON_OBJS =               \


OBJS =                      \
    cal-i-api.o             \
    connect.o               \
    publisher.o             \
    subscriber.o            \
    sendto.o                \
    global.o                \
    make-listening-socket.o \


all: cal-i.a

cal-i.a: cal-i.a($(OBJS) $(COMMON_OBJS))

clean:
	rm -f $(OBJS) cal-i.a

test:

install:

.PHONY: all test install clean

