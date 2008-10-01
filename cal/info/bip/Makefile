
include ../../Makefile.params

CFLAGS += -I ../..

COMMON_OBJS =  $(wildcard ../../shared/*.o)


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
	install -m 0644 cal-i.a $(DESTDIR)/usr/lib/

.PHONY: all test install clean

