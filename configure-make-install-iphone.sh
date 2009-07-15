#!/bin/sh

set -e

if [[ "$0" =~  -iphone-sim.sh$ ]]; then
	echo "Configuring for iPhone Simulator"

	XARCH=i686
	XHOST=i686-apple-darwin
	XDIR=/Developer/Platforms/iPhoneSimulator.platform/Developer
	XSDK=iPhoneSimulator2.2.sdk
	XPORTROOT=/opt/local/i686-apple-darwin

	XCFLAGS="-arch $XARCH -pipe -std=c99 -Wno-trigraphs -fpascal-strings -fasm-blocks -O0 -Wreturn-type -Wunused-variable -fmessage-length=0 -miphoneos-version-min=2.0 -gdwarf-2 -mthumb -miphoneos-version-min=2.0 -I$XDIR/SDKs/$XSDK/usr/include -I$XPORTROOT/usr/include -isysroot /$XDIR/SDKs/$XSDK"
		
	XLDFLAGS="-arch $XARCH -pipe -std=c99 -gdwarf-2 -mthumb -L$XDIR/SDKs/$XSDK/usr/lib -L$XPORTROOT/usr/lib -isysroot /$XDIR/SDKs/$XSDK"

	XDESTDIR=$PWD/client/iphone/bionet_simulator_sdk
else
	echo "Configuring for iPhone"

	XARCH=armv6
	XHOST=arm-apple-darwin
	XDIR=/Developer/Platforms/iPhoneOS.platform/Developer
	XSDK=iPhoneOS2.2.sdk
	XPORTROOT=/opt/local/arm-apple-darwin
	XSYSROOT=$XDIR/SDKs/$XSDK

	XCFLAGS="-arch $XARCH -pipe -std=c99 -Wno-trigraphs -fpascal-strings -fasm-blocks -O0 -Wreturn-type -Wunused-variable -fmessage-length=0 -gdwarf-2 -mthumb -miphoneos-version-min=2.0 -I$XDIR/SDKs/$XSDK/usr/include -I$XPORTROOT/usr/include -isysroot /$XDIR/SDKs/$XSDK"
		
	XLDFLAGS="-arch $XARCH -pipe -std=c99 -gdwarf-2 -mthumb -L$XDIR/SDKs/$XSDK/usr/lib -L$XPORTROOT/usr/lib -isysroot /$XDIR/SDKs/$XSDK"

	XDESTDIR=$PWD/client/iphone/bionet_sdk
	XCACHEFILE=--cache-file=arm-apple-darwin.cache

fi

export PATH=/opt/local/bin:/opt/local/sbin:$XDIR/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/X11/bin

#export PKG_CONFIG_PATH=$XSYSROOT/usr/lib/pkgconfig:$XPORTROOT/lib/pkgconfig
#export PKG_CONFIG_LIBDIR=$XSYSROOT/usr/lib/pkgconfig:$XPORTROOT/lib/pkgconfig
export PKG_CONFIG_PATH=$XPORTROOT/lib/pkgconfig:$XSYSROOT/usr/lib/pkgconfig
export PKG_CONFIG_LIBDIR=$XPORTROOT/lib/pkgconfig:$XSYSROOT/usr/lib/pkgconfig
export PKG_CONFIG=/opt/local/bin/pkg-config

#This is autogen.sh with custom aclocal flags...
aclocal -I$XPORTROOT/share/aclocal -I/opt/local/share/aclocal
autoreconf --force --install


#These use to be exported in the environment, but I don't think they're needed
#export PERL="/opt/local/bin/perl"
#export CC="$XPORTROOT/bin/$XHOST-gcc"
#export CPP="$XPORTROOT/bin/$XHOST-gcc -E"
#export CXX="$XPORTROOT/bin/$XHOST-gcc"
#export INSTALL='/usr/bin/install -c'
#export AR="$XPORTROOT/bin/$XHOST-ar"
#export NM="$XPORTROOT/bin/$XHOST-nm"

#These can't be figured out by autoconf when cross-compiling
export GLIB_CFLAGS=`$PKG_CONFIG --cflags glib-2.0 gthread-2.0`
export GLIB_LIBS=`$PKG_CONFIG --libs glib-2.0 gthread-2.0`


./configure \
	--host=$XHOST \
	--enable-static \
	--disable-python \
	--disable-check \
	"$XCACHEFILE" \
	CFLAGS="$XCFLAGS" \
	LDFLAGS="$XLDFLAGS" \
	CXXFLAGS="$XCFLAGS" \
	CC="/opt/local/bin/$XHOST-gcc" \
	CPP="/opt/local/bin/$XHOST-gcc -E" \
	CXX="/opt/local/bin/$XHOST-gcc" \
	AR="/opt/local/bin/$XHOST-ar" \
	NM="/opt/local/bin/$XHOST-nm" \
	PKG_CONFIG_PATH="$PKG_CONFIG_PATH" \
	PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR" \
	PKG_CONFIG="$PKG_CONFIG" \
	$@

make
echo Installing...
make DESTDIR=$XDESTDIR install
rm -f $XDESTDIR/usr/local/lib/*.dylib
echo Done
