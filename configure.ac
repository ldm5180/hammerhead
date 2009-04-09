
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


# Process this file with autoconf to produce a configure script.

# Prelude.
AC_PREREQ([2.59])
AC_INIT([Bionet], [2.1], [lenny@colorado.edu])

# unique source file --- primitive safety check 
AC_CONFIG_SRCDIR([cal/cal-server.h])

# place to put some extra build scripts installed
AC_CONFIG_AUX_DIR([build-aux])

# fairly severe build strictness
# change foreign to gnu or gnits to comply with gnu standards
AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.9.2])

#AC_CANONICAL_TARGET
#AC_CANONICAL_SYSTEM

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CHECK_TOOL([STRIP],[strip])

CFLAGS=-Wall -Werror -g

AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)

PKG_PROG_PKG_CONFIG

# Checks for libraries.
AC_ARG_ENABLE(check, AC_HELP_STRING([--disable-check],
      [disable check unit testing]),
      [ENABLE_CHECK=$enableval],
      [ENABLE_CHECK=yes])
if test "x$ENABLE_CHECK" = "xyes"; then
PKG_CHECK_MODULES(CHECK, [check >= 0.9.5])
AC_SUBST(CHECK_CFLAGS)
AC_SUBST(CHECK_LIBS)
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
uname=`uname`
case $uname in
     Linux)
     AC_MSG_RESULT(checking OS... Linux)
     HAVE_LINUX=yes
     HAVE_DARWIN=no
     ALSA_ABLE=yes
     ;;
     Darwin)
     AC_MSG_RESULT(checking OS... Darwin/OSX)
     HAVE_DARWIN=yes
     HAVE_LINUX=no
     ALSA_ABLE=no
     ;;
     *)
     echo "[!] Your system architecture is not supported by Bionet v2"
     exit 0
     ;;
esac

# User Selected features template
#AC_ARG_ENABLE([featureX],
#            [AS_HELP_STRING([--enable-featureX],
#              [Enable featureX @<:@default=yes@:>@])],
#            [],
#            [enable_featureX=yes])
#          
#  ENABLE_FEATUREX=
#  AS_IF([test "x$enable_featureX" != xno],
#      [AC_SUBST([ENABLE_FEATUREX], [1])
#       AC_DEFINE([ENABLE_FEATUREX], [1],
#                 [Define if you have want featureX support])
#    ])

PKG_CHECK_MODULES(GLIB20, [ gthread-2.0 >= 2.4.0 ])
AC_SUBST(GLIB20_CFLAGS)
AC_SUBST(GLIB20_LIBS)

PKG_CHECK_MODULES(OPENSSL, [ openssl >= 0.9.8 ])
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)

if test "x$HAVE_DARWIN" = "xno"; then
    PKG_CHECK_MODULES(AVAHI_COMPAT_LIBDNS_SD, [ avahi-compat-libdns_sd >= 0.6.16 ])
    AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_CFLAGS)
    AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_LIBS)
fi

AM_CONDITIONAL(ENABLE_DARWIN, test "x$HAVE_DARWIN" = "xyes")
AM_CONDITIONAL(ENABLE_LINUX, test "x$HAVE_LINUX" = "xyes")


###
#
# see if the user wants ALSA stuff, and if so if it's available, only on Linux
#
###

if test "x$ALSA_ABLE" = "xyes"; then
    AC_ARG_ENABLE(
        alsa,
        AC_HELP_STRING([--disable-alsa], [disable ALSA dependent parts]),
        [ENABLE_ALSA=$enableval],
        [ENABLE_ALSA=yes]
    )
else
    ENABLE_ALSA=no
fi

if test "x$ENABLE_ALSA" = "xyes" ; then
    PKG_CHECK_MODULES(ALSA, [ alsa >= 1.0.16 ])
    AC_SUBST(ALSA_CFLAGS)
    AC_SUBST(ALSA_LIBS)
fi

AM_CONDITIONAL(ENABLE_ALSA, test "x$ENABLE_ALSA" = "xyes")




###
# 
# make sure there is sqlite3 (required for BDM)
#
###

AC_CHECK_LIB(
    sqlite3,
    sqlite3_libversion_number,
    [ HAVE_SQLITE3=yes ],
    [ AC_MSG_ERROR([could not find libsqlite3-dev]) ],
    [ ]
)


#
# Check for isfinite() in <math.h>. isfinite() is a c99 standard,
# but finite() is the older BSD standard is isfinite is not availble
AC_CHECK_DECL(isfinite, 
  [AC_DEFINE([HAVE_ISFINITE],[1],[<math.h> provides isfinite])], 
  , 
  [[#include <math.h>]])



AC_ARG_ENABLE(python, AC_HELP_STRING([--disable-python],
      [disable python dependent parts]),
      [ENABLE_PYTHON=$enableval],
      [ENABLE_PYTHON=yes])
if test "x$ENABLE_PYTHON" = "xyes" ; then
   AC_PATH_PROG(PYTHON, python, no)
   AC_PATH_PROG(SWIG, swig, no)
   if test "x$PYTHON" = xno; then
      AC_MSG_ERROR([could not find python])
   fi
   if test "x$SWIG" = xno; then
      AC_MSG_ERROR([could not find swig])
   fi
fi
AM_CONDITIONAL(ENABLE_PYTHON, test "x$ENABLE_PYTHON" = "xyes")

AC_ARG_ENABLE(qt4, AC_HELP_STRING([--disable-qt4],
      [disable QT4 dependent parts]),
      [ENABLE_QT4=$enableval],
      [ENABLE_QT4=yes])
if test "x$HAVE_LINUX" = "xyes"; then
   if test "x$ENABLE_QT4" = "xyes" ; then
      AC_PATH_PROG(QT4_PATH, qmake-qt4, no)
      if test "x$QT4_PATH" = xno; then
       	 AC_MSG_ERROR([could not find QT4 executables])
      fi
   fi
fi
if test "x$HAVE_DARWIN" = "xyes"; then
   if test "x$ENABLE_QT4" = "xyes" ; then
      AC_PATH_PROG(QT4_PATH, qmake-4.4, no)
      if test "x$QT4_PATH" = xno; then
       	 AC_MSG_ERROR([could not find QT4 executables])
      fi
   fi
fi

# need libqwt5 for mac...
#if test "x$HAVE_DARWIN" = "xyes"; then
#   if test "x$ENABLE_QT4" = "xyes" ; then
#      AC_PATH_PROG(QT4_PATH, qmake-4.4, no)
#      if test "x$QT4_PATH" = xno; then
#         AC_MSG_ERROR([could not find QT4 executables])
#      fi
#   fi
#fi
if test "x$HAVE_LINUX" = "xyes"; then
AM_CONDITIONAL(ENABLE_QT4, [test "x$ENABLE_QT4" = "xyes"])
else
AM_CONDITIONAL(ENABLE_QT4, [test "xyes" = "xno"]) #this will always be false
fi

if test "x$HAVE_DARWIN" = "xyes"; then
    echo "checking for LTKC... disabled"
    AM_CONDITIONAL(ENABLE_LTKC, test "x$HAVE_DARWIN" = "false")
else
    echo "checking for LTKC... yes"
    AM_CONDITIONAL(ENABLE_LTKC, test "x$HAVE_LINUX" = "x$HAVE_LINUX")
fi

if test "x$TOSROOT" = "x"; then
echo "checking for TinyOS... no"
else
echo "checking for TinyOS... yes"
fi
AM_CONDITIONAL([TINYOSMIG_DISABLED], [test "x$TOSROOT" = "x"])

#doxygen
AC_PATH_PROG(DOXYGEN, doxygen, no)
AM_CONDITIONAL([ENABLE_DOXYGEN], [test "x$DOXYGEN" != "xno"])

#ccache
AC_PATH_PROG(CCACHE, ccache, no)
AM_CONDITIONAL([ENABLE_CCACHE], [test "x$CCACHE" != "xno"])
if test "x$CCACHE" != "xno"; then
   CC="ccache $CC"
   CXX="ccache $CXX"
fi


AC_PATH_PROG(DOT, dot, no)
AM_CONDITIONAL([DISABLE_DOT], [test "x$DOT" = "xno"])
		      
#bluetooth
AC_ARG_ENABLE(bluetooth, AC_HELP_STRING([--disable-bluetooth],
      [disable bluetooth dependent parts]),
      [ENABLE_BLUETOOTH=$enableval],
      [ENABLE_BLUETOOTH=yes])
if test "x$HAVE_LINUX" = "xyes"; then
    if test "x$ENABLE_BLUETOOTH" = "xyes"; then
        PKG_CHECK_MODULES(BLUEZ, [ bluez >= 0.22 ])
        AC_SUBST(BLUEZ_CFLAGS)
        AC_SUBST(BLUEZ_LIBS)
        AM_CONDITIONAL([ENABLE_BLUETOOTH], [test "x$ENABLE_BLUETOOTH" = "xyes"])	
    else
        AM_CONDITIONAL([ENABLE_BLUETOOTH], [test "x$ENABLE_BLUETOOTH" = "xyes"])
    fi
else
    AM_CONDITIONAL([ENABLE_BLUETOOTH], [test "x$ENABLE_BLUETOOTH" = "false"])
fi

#xsltproc
AC_ARG_ENABLE(xslt, AC_HELP_STRING([--disable-xslt],
      [disable xslt dependent parts]),
      [ENABLE_XSLT=$enableval],
      [ENABLE_XSLT=yes])
if test "x$ENABLE_XSLT" = "xyes"; then
   AC_PATH_PROG(XSLTPROC, xsltproc, no)
   AM_CONDITIONAL([HAVE_XSLTPROC], [test "x$XSLTPROC" != "xno"])
fi

# Checks for library functions.
AC_FUNC_MALLOC

# Output files
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile
                 cal/Makefile
		 cal/util/Makefile
		 cal/mdnssd-bip/Makefile
		 cal/mdnssd-bip/shared/Makefile
		 cal/mdnssd-bip/client/Makefile
		 cal/mdnssd-bip/server/Makefile
		 cal/examples/Makefile
                 asn.1/Makefile
                 util/Makefile
                 client/Makefile
                 client/libbionet/Makefile
                 client/libbionet-qt4/Makefile
		 client/monitor/Makefile
                 client/py-libbionet/Makefile
                 client/watcher/Makefile
                 client/stream/Makefile
		 client/stream-manager/Makefile
                 hab/Makefile
                 hab/libhab/Makefile
		 hab/py-libhab/Makefile
		 hab/libamedevice/Makefile
                 hab/csa-cp/Makefile
                 hab/cpod/Makefile
		 hab/mmod/Makefile
                 hab/random/Makefile
                 hab/alsa/Makefile
                 hab/parsec/Makefile
		 hab/stethoscope/Makefile
                 hab/syshealth/Makefile
                 hab/speedway/Makefile
                 hab/pal-650/Makefile
                 hab/uwb/Makefile
                 hab/streamy/Makefile
                 data-manager/Makefile
                 data-manager/server/Makefile
                 data-manager/client/Makefile
                 data-manager/py-client/Makefile
		 check/Makefile
		 check/libutil/Makefile
                 ])

AC_OUTPUT
