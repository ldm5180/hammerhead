
include ../Makefile.params

CFLAGS += -I .

OBJS =                          \
    event-new.o                 \
    event-free.o                \
    peer-new.o                  \
    peer-address.o              \
    peer-free.o                 \


all: $(OBJS)

clean:
	rm -f $(OBJS)

test:

install:
	install -m 0644 cal-util.h $(DESTDIR)/usr/include/
	install -m 0644 cal-peer.h $(DESTDIR)/usr/include/
	install -m 0644 cal-event.h $(DESTDIR)/usr/include/

.PHONY: all test install clean

