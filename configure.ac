
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


# Process this file with autoconf to produce a configure script.

# Prelude.
AC_PREREQ([2.59])
AC_INIT([Bionet], [2.3], [bionet@bioserve.colorado.edu])

# unique source file --- primitive safety check 
AC_CONFIG_SRCDIR([cal/cal-server.h])

# place to put some extra build scripts installed
AC_CONFIG_AUX_DIR([build-aux])

# local m4 scripts
AC_CONFIG_MACRO_DIR([build-aux/m4])

# fairly severe build strictness
# change foreign to gnu or gnits to comply with gnu standards
AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.9.2])

#AC_CANONICAL_TARGET
#AC_CANONICAL_SYSTEM

# Checks for programs.
# We don't want to use the default AC_PROG_* flags set by these macros,
# but we do want to allow the user to specify custom flags.
# Save the user's settings, and override the default set by AC_PROG_*
USERCFLAGS="$CFLAGS"
USERCPPFLAGS="$CFLAGS"
USERCXXFLAGS="$CFLAGS"
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_YACC
AM_PROG_LEX

CFLAGS="$USERCFLAGS"
CPPFLAGS="$USERCPPFLAGS"
CXXFLAGS="$USERCXXFLAGS"


AC_CHECK_TOOL([STRIP],[strip])

AC_SUBST(docdir)

# Checks for libraries.
AC_ARG_ENABLE(check, AC_HELP_STRING([--disable-check],
      [disable check unit testing]),
      [ENABLE_CHECK=$enableval],
      [ENABLE_CHECK=yes])
if test "x$ENABLE_CHECK" = "xyes"; then
PKG_CHECK_MODULES([CHECK], [ check >= 0.9.3 ])
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

if test "x$host_alias" != x; then
  if test "x$build_alias" = x; then
    cross_compiling=maybe
    $as_echo "$as_me: WARNING: If you wanted to set the --build type, don't use --host.
    If a cross compiler is detected then cross compile mode will be used." >&2
    AM_CONDITIONAL(CROSS_COMPILING, test "x" = "y")
  elif test "x$build_alias" != "x$host_alias"; then
    cross_compiling=yes
    AM_CONDITIONAL(CROSS_COMPILING, test "x" = "x")
  fi
else
    AM_CONDITIONAL(CROSS_COMPILING, test "x" = "y")
fi

if test "x$GLIB_CFLAGS" = "x" && test "x$GLIB_LIBS" = "x"; then
    if test "$cross_compiling" != yes; then
    AM_PATH_GLIB_2_0([2.12.0], 
        [], [], [gthread])
    fi
fi
if test "x$GLIB_CFLAGS" = "x" && test "x$GLIB_LIBS" = "x"; then
    AC_MSG_ERROR([GLIB required to build], [1])
fi
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

if test "$cross_compiling" != yes; then
PKG_CHECK_MODULES(OPENSSL, [ openssl >= 0.9.8 ])
fi
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)

if test "$cross_compiling" != yes; then
if test "x$HAVE_DARWIN" = "xno"; then
    PKG_CHECK_MODULES(AVAHI_COMPAT_LIBDNS_SD, [ avahi-compat-libdns_sd >= 0.6.16 ])
    AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_CFLAGS)
    AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_LIBS)
fi
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


# disable HABs
AC_ARG_ENABLE(
    habs,
    AC_HELP_STRING([--disable-habs], [disable Bionet HABs]),
        [ENABLE_HABS=$enableval],
        [ENABLE_HABS=yes]
    )
AM_CONDITIONAL(ENABLE_HABS, test "x$ENABLE_HABS" = "xyes")

# disable Clients
AC_ARG_ENABLE(
    clients,
    AC_HELP_STRING([--disable-clients], [disable Bionet Clients]),
        [ENABLE_CLIENTS=$enableval],
        [ENABLE_CLIENTS=yes]
    )
AM_CONDITIONAL(ENABLE_CLIENTS, test "x$ENABLE_CLIENTS" = "xyes")


###
# 
# make sure there is sqlite3 (required for BDM)
#
###
AC_ARG_ENABLE(
    bdm,
    AC_HELP_STRING([--disable-bdm], [disable Bionet Data Manager]),
        [ENABLE_BDM=$enableval],
        [ENABLE_BDM=yes]
    )
if test "x$ENABLE_BDM" = "xyes"; then
    AC_CHECK_LIB(
        sqlite3,
        sqlite3_libversion_number,
        [ HAVE_SQLITE3=yes ],
        [ AC_MSG_ERROR([could not find libsqlite3-dev]) ],
        [ ]
    )


fi
AM_CONDITIONAL(ENABLE_BDM, test "x$ENABLE_BDM" = "xyes")

AC_ARG_WITH(
    [ion],
    AC_HELP_STRING([--with-ion], [Use ION for BDM syncronization over DTN @<:@default=no@:>@]),
        [with_ion=$withval],
        [with_ion=no]
    )

AS_IF([test "x$with_ion" != "xno"],
    [AC_DEFINE([ENABLE_ION], 1, [Define if you have BP support via ION]) 
     AC_SUBST([ENABLE_ION], 1)
     if test "$cross_compiling" != yes; then
# Search for ION libs/Headers, and modify CFLAGS/LDFLAGS if needed 
#(These all add to the list of libs to link into future tests)
        AX_EXT_HAVE_HEADER([ion.h], [/usr/include/ion /usr/local/include /opt/include]) 
        AX_EXT_HAVE_LIB([/usr/lib/ion /usr/local/lib /opt/lib], [bp], [bp_attach], [-lbpP -lrfx -lici])

# Check if the bioserve features are present
        AC_CHECK_FUNCS([bp_add_endpoint sm_set_basekey])
     fi
     ])
AM_CONDITIONAL(ENABLE_ION, [test "x$with_ion" != "xno"])

#
# optionally compile the mmod hab
#
AC_ARG_ENABLE(
    mmod,
    AC_HELP_STRING([--disable-mmod], [disable MMOD HAB]),
        [ENABLE_MMOD=$enableval],
        [ENABLE_MMOD=yes]
    )
AM_CONDITIONAL(ENABLE_MMOD, test "x$ENABLE_MMOD" = "xyes")


#
# optionally compile the parsec hab
#
AC_ARG_ENABLE(
    parsec,
    AC_HELP_STRING([--disable-parsec], [disable Parsec HAB]),
        [ENABLE_PARSEC=$enableval],
        [ENABLE_PARSEC=yes]
    )
AM_CONDITIONAL(ENABLE_PARSEC, test "x$ENABLE_PARSEC" = "xyes")


#
# optionally compile the uwb hab
#
AC_ARG_ENABLE(
    uwb,
    AC_HELP_STRING([--disable-uwb], [disable UWB HAB]),
        [ENABLE_UWB=$enableval],
        [ENABLE_UWB=yes]
    )
AM_CONDITIONAL(ENABLE_UWB, test "x$ENABLE_UWB" = "xyes")


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
   AM_PATH_PYTHON([2.5])
   AC_PROG_SWIG
   SWIG_PYTHON
fi
AM_CONDITIONAL(ENABLE_PYTHON, test "x$ENABLE_PYTHON" = "xyes")

# QT and qt libs
AC_ARG_ENABLE(qt4, AC_HELP_STRING([--disable-qt4],
      [disable QT4 dependent parts]),
      [ENABLE_QT4=$enableval],
      [ENABLE_QT4=yes])

if test "x$ENABLE_QT4" = "xyes" ; then
    AC_CACHE_CHECK([for qmake for QT4], [ac_cv_path_QMAKE],
        [AC_PATH_PROGS_FEATURE_CHECK([QMAKE], [qmake-qt4 qmake-4.4 qmake qmake-mac],
            [[$ac_path_QMAKE -version | grep -iq 'qt version 4' && ac_cv_path_QMAKE=$ac_path_QMAKE ac_path_QMAKE_found=:]],
            [AC_MSG_ERROR([could not find qmake for QT4 ])])])
    AC_SUBST([QMAKE], [$ac_cv_path_QMAKE])

fi

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
AC_ARG_ENABLE(doxygen, AC_HELP_STRING([--disable-doxygen],
      [disable doxygen documentation generation]),
      [ENABLE_DOXYGEN=$enableval],
      [ENABLE_DOXYGEN=yes])
if test "x$ENABLE_DOXYGEN" = "xyes" ; then
   AC_PATH_PROG(DOXYGEN, doxygen, no)
   AM_CONDITIONAL([ENABLE_DOXYGEN], [test "x$DOXYGEN" != "xno"])
else
   AM_CONDITIONAL([ENABLE_DOXYGEN], [test "x" = "y"])
fi


#ccache
AC_PATH_PROG(CCACHE, ccache, no)
AM_CONDITIONAL([ENABLE_CCACHE], [test "x$CCACHE" != "xno"])
if test "x$CCACHE" != "xno"; then
   CC="ccache $CC"
   CXX="ccache $CXX"
fi

#bazaar
AC_PATH_PROG(BAZAAR, bzr, no)
AM_CONDITIONAL([DISABLE_BZR], [test "x$BAZAAR" = "xno"])
if test "x$BAZAAR" = "xno"; then
   test "$BIONET_BUILD_TAG" == "" && BIONET_BUILD_TAG="+$USER"
else
   test "$BIONET_BUILD_TAG" == "" && BIONET_BUILD_TAG="+`bzr revno`+$USER"
fi
AC_ARG_VAR([BIONET_BUILD_TAG], [Bionet build ID])

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
AM_CONDITIONAL([HAVE_XSLTPROC], [test "x" == "y"])
AC_ARG_ENABLE(xslt, AC_HELP_STRING([--disable-xslt],
      [disable xslt dependent parts]),
      [ENABLE_XSLT=$enableval],
      [ENABLE_XSLT=yes])
if test "x$ENABLE_XSLT" = "xyes"; then
   AC_PATH_PROG(XSLTPROC, xsltproc, no)
   AM_CONDITIONAL([HAVE_XSLTPROC], [test "x$XSLTPROC" != "xno"])
fi

#bison & flex
AM_CONDITIONAL([ENABLE_BISON], [test "x$ac_cv_prog_YACC" != "x"])
AM_CONDITIONAL([ENABLE_FLEX], [test "x$ac_cv_prog_LEX" != "x"])

# GWT UIs depend on Java at compile-time.
AC_PATH_PROG([JAVA], [java], [no], [])
AM_CONDITIONAL([ENABLE_JAVA], [test "x$JAVA" != "xno"])

# Google Web Toolkit is required for compiling the webstats app from Java
# into JavaScript.  Both the GWT jars and java (not javac) are required.
# GWT doesn't have a normal system path, so the user has to pass a path.
AC_ARG_WITH([gwt], AC_HELP_STRING([--with-gwt],
    [Path to Google Web Toolkit to build web UIs @<:@default=none@:>@]),
    [if test "x$JAVA" == "xno"; then
		AC_MSG_ERROR([--with-gwt specified but java could not be found], [1])
	 fi
	 AC_SUBST([GWT_PATH], [$withval])],
    [with_gwt=none])
AM_CONDITIONAL([ENABLE_GWT], [test "x$with_gwt" != "xnone"])

# Checks for library functions.
if test "$cross_compiling" != yes; then
AC_FUNC_MALLOC
fi

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
		 util/version.c
                 client/Makefile
                 client/libbionet/Makefile
                 client/libbionet-qt4/Makefile
		 client/monitor/Makefile
                 client/py-libbionet/Makefile
                 client/watcher/Makefile
                 client/stream/Makefile
		 client/stream-manager/Makefile
		 client/commander/Makefile
                 hab/Makefile
                 hab/libhab/Makefile
		 hab/py-libhab/Makefile
		 hab/libamedevice/Makefile
                 hab/csa-cp/Makefile
                 hab/cpod/Makefile
		 hab/mmod/Makefile
                 hab/random/Makefile
                 hab/test-pattern/Makefile
                 hab/alsa/Makefile
                 hab/parsec/Makefile
		 hab/stethoscope/Makefile
                 hab/syshealth/Makefile
                 hab/speedway/Makefile
                 hab/pal-650/Makefile
                 hab/uwb/Makefile
                 hab/streamy/Makefile
                 hab/bpstats2/Makefile
                 data-manager/Makefile
                 data-manager/server/Makefile
                 data-manager/client/Makefile
                 data-manager/py-client/Makefile
                 data-manager/bdmplot/Makefile
		 security/Makefile
		 security/ca/Makefile
		 apps/Makefile
		 apps/webstats/Makefile
		 test/config.sh
		 check/Makefile
		 check/libutil/Makefile
		 check/data-manager/Makefile
		 check/asn1/Makefile
                 ])

AC_OUTPUT
