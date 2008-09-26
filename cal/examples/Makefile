
include ../Makefile.params

CFLAGS += -I .. -I ../peer-discovery -I ../info
LDLIBS = -lpthread `pkg-config --libs glib-2.0`

ifeq (${TARGET_OS},LINUX)
dnssd_LDLIBS = `pkg-config --libs avahi-compat-libdns_sd`
endif


CAL_PD_MODULES =  \
    dnssd         \
    bdp           \

CAL_I_MODULES =  \
    bip          \


all: time-publisher time-subscriber




#
# oh make, what am I going to do with you?
#

define template

time-publisher-$(1)-$(2): time-publisher.o ../peer-discovery/$(1)/cal-pd.a ../info/$(2)/cal-i.a # $$($(1)_OBJS) $$($(1)_LIBS:%=-l%)
	$(CC) $(LDFLAGS) $$^ -o $$@ $(LDLIBS) $($(1)_LDLIBS) $($(2)_LDLIBS)

time-subscriber-$(1)-$(2): time-subscriber.o ../peer-discovery/$(1)/cal-pd.a ../info/$(2)/cal-i.a # $$($(1)_OBJS) $$($(1)_LIBS:%=-l%)
	$(CC) $(LDFLAGS) $$^ -o $$@ $(LDLIBS) $($(1)_LDLIBS) $($(2)_LDLIBS)

endef

$(foreach PD,$(CAL_PD_MODULES),$(foreach I,$(CAL_I_MODULES),$(eval $(call template,$(PD),$(I)))))




time-publisher: $(foreach PD,$(CAL_PD_MODULES),$(foreach I,$(CAL_I_MODULES),time-publisher-$(PD)-$I))

time-subscriber: $(foreach PD,$(CAL_PD_MODULES),$(foreach I,$(CAL_I_MODULES),time-subscriber-$(PD)-$I))


clean:
	set -e; for PD in $(CAL_PD_MODULES); do                          \
	    for I in $(CAL_I_MODULES); do                                \
	        rm -f time-publisher-$$PD-$$I time-subscriber-$$PD-$$I;  \
	    done                                                         \
	done

	rm -f time-subscriber.o time-publisher.o


test:

install:


.PHONY: all time-subscriber time-publisher test install clean

