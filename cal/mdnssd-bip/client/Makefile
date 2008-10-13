
include ../../Makefile.params

CFLAGS += -I .. -I ../.. -I ../../util

COMMON_OBJS =  $(wildcard ../../util/*.o) $(wildcard ../shared/*.o)


OBJS =                      \
    cal-client-api.o        \
    global.o                \
    thread-functions.o      \


all: cal-client.a

cal-client.a: cal-client.a($(OBJS) $(COMMON_OBJS))

clean:
	rm -f $(OBJS) cal-client.a

test:

install:
	install -m 0644 cal-client.a $(DESTDIR)/usr/lib/

.PHONY: all test install clean


