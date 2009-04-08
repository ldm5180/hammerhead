#!/bin/bash

# This wrapper will inspect the command it was invoked with, 
# determine the correct build tool to invoke and add necessary options
# to make the xcode tools behave like standard cross-compile tools
# (i.e. that have sysroots and architectures built in...

if [[ $0 =~ (.*)-apple-darwin-(.*) ]]; then
	arch=${BASH_REMATCH[1]};
	TOOL=${BASH_REMATCH[2]};
else
	echo "Apple architecture expected (e.g. arm-apple-darwin)"
	exit 1
fi 


case $arch in
	*arm)
		XARCH=armv6
		XHOST=arm-apple-darwin
		XDIR=/Developer/Platforms/iPhoneOS.platform/Developer
		XSDK=iPhoneOS2.2.sdk
		XPORTROOT=/opt/local/arm-apple-darwin
		;;
	*i?86)
		XARCH=i686
		XHOST=i686-apple-darwin
		XDIR=/Developer/Platforms/iPhoneSimulator.platform/Developer
		XSDK=iPhoneSimulator2.2.sdk
		XPORTROOT=/opt/local/i686-apple-darwin
		;;
	*)
		echo "Apple architecture expected (e.g. arm-apple-darwin)"
		exit 1
esac

SYSROOT=$XDIR/SDKs/$XSDK
export IPHONEOS_DEPLOYMENT_TARGET=2.2

case $TOOL in
	gcc|g++|ld)
		#CMD="$XDIR/usr/bin/$TOOL -arch $XARCH -isysroot $SYSROOT $@ -I$SYSROOT/usr/include -L$SYSROOT/usr/lib"
		CMD="$XDIR/usr/bin/$TOOL -arch $XARCH -isysroot $SYSROOT -I$SYSROOT/usr/include -I$XDIR/usr/lib/gcc/arm-apple-darwin9/4.0.1/include -L$SYSROOT/usr/lib $@"
		;;
	*)
		CMD="$TOOL $@"
		;;
esac

#echo "--> $CMD" >&2
exec $CMD

