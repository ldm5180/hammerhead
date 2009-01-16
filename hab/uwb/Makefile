# Makefile for Time Domain P-210 Radio
#
# G.L. Grobe
# 

-include ../../../Makefile.params
include ../../Makefile.params

TARGET = td-p210-hab

CFLAGS  := -I../libhab -I../../util ${CFLAGS} `pkg-config --cflags glib-2.0`
LDFLAGS := -L../libhab ${LDFLAGS}
LDLIBS = -lhab `pkg-config --libs glib-2.0`

ifeq (${TARGET_OS},WINDOWS)
    LDLIBS += -lwsock32
endif

SOURCE_FILES = 				\
	td-radio-hab.c			\
	global.c				\
	process-data.c			\
	td-radio-connect.c		\
	td-radio-read.c			\
	td-radio-send.c			\
	make-shutdowns-clean.c 	\
	cb-set-resource.c

all: $(TARGET)

$(TARGET): $(SOURCE_FILES:%.c=%.o)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

#install: $(TARGET)
#	$(INSTALL) -m 0755 $(TARGET) ${EXP_BIN_DIR}
#	$(INSTALL) -m 0755 init.d.$(TARGET) ${EXP_INIT_DIR}/$(TARGET)
#	$(INSTALL) -m 0644 $(TARGET).conf   ${EXP_ETC_DIR}/$(TARGET).conf

clean:
	rm -f $(TARGET) *.o


.PHONY:  all install clean


