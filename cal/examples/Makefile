
include ../Makefile.params

CFLAGS += -I .. -I ../util -I ../client -I ../info
LDLIBS = -lpthread `pkg-config --libs glib-2.0`

ifeq (${TARGET_OS},LINUX)
mdnssd-bip_LDLIBS = `pkg-config --libs avahi-compat-libdns_sd`
endif


CAL_MODULES = $(shell cat ../CAL-Modules)


all: time-publisher time-subscriber




#
# oh make, what am I going to do with you?
#

define template

time-publisher-$(1): time-publisher.o ../$(1)/server/cal-server.a
	$(CC) $(LDFLAGS) $$^ -o $$@ $(LDLIBS) $($(1)_LDLIBS)

time-subscriber-$(1): time-subscriber.o ../$(1)/client/cal-client.a
	$(CC) $(LDFLAGS) $$^ -o $$@ $(LDLIBS) $($(1)_LDLIBS)

endef

$(foreach MODULE,$(CAL_MODULES),$(eval $(call template,$(MODULE))))




time-publisher: $(foreach MODULE,$(CAL_MODULES),time-publisher-$(MODULE))

time-subscriber: $(foreach MODULE,$(CAL_MODULES),time-subscriber-$(MODULE))


clean:
	set -e; for MODULE in $(CAL_MODULES); do                      \
	    rm -f time-publisher-$$MODULE time-subscriber-$$MODULE;   \
	done

	rm -f time-subscriber.o time-publisher.o


test:

install: time-publisher time-subscriber
	set -e; for MODULE in $(CAL_MODULES); do                            \
	    install -m 0755 time-publisher-$$MODULE $(DESTDIR)/usr/bin/;    \
	    install -m 0755 time-subscriber-$$MODULE $(DESTDIR)/usr/bin/;   \
	done


.PHONY: all time-subscriber time-publisher test install clean

