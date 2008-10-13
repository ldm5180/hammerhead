
include ../Makefile.params


SUBDIRS =       \
    shared      \
    client      \
    server      \


all test install clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


.PHONY: all test install clean

