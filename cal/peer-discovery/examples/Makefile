
include ../../Makefile.params

CFLAGS += -I ../..
LDLIBS = -lpthread `pkg-config --libs glib-2.0`

ifeq (${TARGET_OS},LINUX)
dnssd_LDLIBS = `pkg-config --libs avahi-compat-libdns_sd`
endif


MODULES = $(shell cat ../CAL-PD-Modules-list)


all: cal-pd-announcer cal-pd-listener hab client


cal-pd-announcer: $(foreach M,$(MODULES),cal-pd-$M-announcer)

cal-pd-%-announcer: announcer.o ../%/cal-pd.a
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $($(patsubst cal-pd-%-announcer,%,$@)_LDLIBS)


cal-pd-listener: $(foreach M,$(MODULES),cal-pd-$M-listener)

cal-pd-%-listener: listener.o ../%/cal-pd.a
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $($(patsubst cal-pd-%-listener,%,$@)_LDLIBS)


hab: $(foreach M,$(MODULES),hab-$M)

hab-%: main-of-hab.o glib-on-cal.o ../%/cal-pd.a
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $($(patsubst hab-%,%,$@)_LDLIBS)


client: $(foreach M,$(MODULES),client-$M)

client-%: main-of-client.o glib-on-cal.o ../%/cal-pd.a
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $($(patsubst client-%,%,$@)_LDLIBS)


clean:
	set -e; for i in $(MODULES); do                      \
	    rm -f cal-pd-$$i-announcer cal-pd-$$i-listener;  \
	    rm -f hab-$$i client-$$i;                        \
	done

	rm -f announcer.o listener.o
	rm -f main-of-hab.o main-of-client.o glib-on-cal.o


test:

install:
	set -e; for i in $(MODULES); do                                 \
	    install -m 0755 cal-pd-$$i-announcer $(DESTDIR)/usr/bin/;   \
	    install -m 0755 cal-pd-$$i-listener  $(DESTDIR)/usr/bin/;   \
	    install -m 0755 hab-$$i              $(DESTDIR)/usr/bin/;   \
	    install -m 0755 client-$$i           $(DESTDIR)/usr/bin/;   \
	done


.PHONY: all modules cal-pd-announcer cal-pd-listener test install clean

