

include ../../Makefile.params

CFLAGS += -I .. -I ../.. -I ../../util

COMMON_OBJS =  $(wildcard ../../util/*.o) $(wildcard ../shared/*.o)

OBJS =                      \
    cal-server-api.o        \
    global.o                \
    thread-functions.o      \


all: cal-server.a

cal-server.a: cal-server.a($(OBJS) $(COMMON_OBJS))

clean:
	rm -f $(OBJS) cal-server.a

test:

install:
	install -m 0644 cal-server.a $(DESTDIR)/usr/lib/

.PHONY: all test install clean


