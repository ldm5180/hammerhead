#!/bin/sh
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#

./configure \
    --enable-static \
    --disable-shared \
    -build=i386 \
    --host=none-uclinux-uclibc \
    --disable-check \
    --disable-alsa \
    --disable-python \
    --disable-qt4 \
    --disable-bluetooth \
    --disable-xslt \
    --disable-bdm \
    --disable-mmod \
    --disable-parsec \
    --disable-uwb \
    --disable-doxygen \
    CC='ucfront microblaze-uclinux-gcc' \
    LDFLAGS='-Wl, -elf2flt' \
    GLIB20_CFLAGS=-I$PETALINUX/software/petalinux-dist/glib/include/glib-2.0 \
    GLIB20_LIBS="$PETALINUX/software/petalinux-dist/glib/lib/libglib-2.0.la $PETALINUX/software/petalinux-dist/glib/lib/libgthread-2.0.la" \
    DBUS_CFLAGS="-I$PETALINUX/software/petalinux-dist/dbus/include/dbus-1.0 -I$PETALINUX/software/petalinux-dist/dbus/lib/dbus-1.0/include" \
    DBUS_LIBS=$PETALINUX/software/petalinux-dist/dbus/lib/libdbus-1.la \
    AVAHI_COMPAT_LIBDNS_SD_CFLAGS=-I$PETALINUX/software/petalinux-dist/avahi/include/avahi-compat-libdns_sd \
    AVAHI_COMPAT_LIBDNS_SD_LIBS=$PETALINUX/software/petalinux-dist/avahi/lib/libdns_sd.la \
    LIBS=$PETALINUX/software/petalinux-dist/dbus/lib/libdbus-1.a \
    OPENSSL_CFLAGS=-I$PETALINUX/software/petalinux-dist/openssl/include \
    OPENSSL_LIBS="$PETALINUX/software/petalinux-dist/openssl/lib/libssl.a $PETALINUX/software/petalinux-dist/openssl/lib/libcrypto.a"

