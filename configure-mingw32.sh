#!/bin/sh

#export PKG_CONFIG_PATH=/home/bmarshall/src/glib2/lib/pkgconfig
export PKG_CONFIG_LIBDIR=/home/bmarshall/src/glib2/lib/pkgconfig

#export GLIB_CFLAGS=`$PKG_CONFIG --cflags glib-2.0 gthread-2.0`
#export GLIB_LIBS=`$PKG_CONFIG --libs glib-2.0 gthread-2.0`

export OPENSSL_LIBS=-L/home/bmarshall/src/openssl/OpenSSL/lib
export OPENSSL_CFLAGS=-I/home/bmarshall/src/openssl/OpenSSL/include

./configure \
    --host=i586-mingw32msvc \
    --disable-habs \
    --disable-clients \
    --disable-bdm \
    --disable-python \
    --disable-check \
    --disable-qt4 \
    --disable-alsa \
    --disable-bluetooth \
    --without-ion
    

