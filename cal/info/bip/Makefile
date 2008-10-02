
include ../../Makefile.params

CFLAGS += -I .. -I ../.. -I ../../util -I ../../peer-discovery

COMMON_OBJS =  $(wildcard ../../util/*.o)


OBJS =                      \
    cal-i-api.o             \
    connect.o               \
    publisher.o             \
    subscriber.o            \
    sendto.o                \
    global.o                \


all: cal-i.a

cal-i.a: cal-i.a($(OBJS) $(COMMON_OBJS))

clean:
	rm -f $(OBJS) cal-i.a

test:

install:
	install -m 0644 cal-i.a $(DESTDIR)/usr/lib/

.PHONY: all test install clean

