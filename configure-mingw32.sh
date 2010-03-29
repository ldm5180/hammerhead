#!/bin/sh

#export PKG_CONFIG_PATH=/home/bmarshall/src/glib2/lib/pkgconfig
#export PKG_CONFIG_LIBDIR=/home/bmarshall/src/glib2/lib/pkgconfig
export PKG_CONFIG_LIBDIR=/usr/i586-mingw32msvc/lib/pkgconfig

#export GLIB_CFLAGS=`$PKG_CONFIG --cflags glib-2.0 gthread-2.0`
#export GLIB_LIBS=`$PKG_CONFIG --libs glib-2.0 gthread-2.0`

#export OPENSSL_LIBS="-L/home/bmarshall/src/openssl/OpenSSL/lib/MinGW -lssleay32 -llibeay32"

#export OPENSSL_LIBS="/home/bmarshall/src/openssl/OpenSSL/lib/MinGW/ssleay32.a /home/bmarshall/src/openssl/OpenSSL/lib/MinGW/libeay32.a"
#export OPENSSL_CFLAGS=-I/home/bmarshall/src/openssl/OpenSSL/include

export AVAHI_COMPAT_LIBDNS_SD_LIBS="-L/home/bmarshall/src/bonjour/sdk/lib/win32 -ldnssd"
export AVAHI_COMPAT_LIBDNS_SD_CFLAGS=-I/home/bmarshall/src/bonjour/sdk/include


#../../glib2.bzr/glib-2.22.4/mingw-configure 

./configure \
    --host=i586-mingw32msvc \
    --prefix=/usr/i586-mingw32msvc \
    --disable-ccache \
    --disable-exe \
    --disable-habs \
    --disable-clients \
    --disable-bdm \
    --disable-python \
    --disable-check \
    --disable-qt4 \
    --disable-alsa \
    --disable-bluetooth \
    --without-ion \
    --enable-shared
    

