
include ../Makefile.params


MODULES = $(shell cat CAL-I-Modules-list)

SUBDIRS =       \
    $(MODULES)  \

#    shared      \
#    examples    \


all clean test install:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

.PHONY: all test install clean

