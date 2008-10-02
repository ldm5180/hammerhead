
include Makefile.params


SUBDIRS =           \
    util            \
    peer-discovery  \
    info            \
    examples        \


all test clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


install:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done
	install -m 0644 cal.h $(DESTDIR)/usr/include/


.PHONY: all test install clean

