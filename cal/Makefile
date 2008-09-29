
include Makefile.params


SUBDIRS =           \
    peer-discovery  \
    info            \
    examples        \


all test install clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


install:
	install -m 0644 cal.h $(DESTDIR)/usr/include/


.PHONY: all test install clean

