
include Makefile.params


MODULES = $(shell cat CAL-Modules)


SUBDIRS =           \
    util            \
    $(MODULES)      \
    examples        \


all test clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


install:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done
	install -m 0644 cal-client.h $(DESTDIR)/usr/include/
	install -m 0644 cal-server.h $(DESTDIR)/usr/include/


.PHONY: all test install clean

