# Process this file with autoconf to produce a configure script.

# Prelude.
AC_PREREQ([2.59])
AC_INIT([bionet], [2.0], [lenny@colorado.edu])

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
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CHECK_TOOL([STRIP],[strip])

# Checks for libraries.

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
     ;;
     Darwin)
     AC_MSG_RESULT(checking OS... Darwin/OSX)
     HAVE_DARWIN=yes
     HAVE_LINUX=no
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

PKG_CHECK_MODULES(GLIB20, [ glib-2.0 >= 2.4.0 ])
AC_SUBST(GLIB20_CFLAGS)
AC_SUBST(GLIB20_LIBS)

PKG_CHECK_MODULES(OPENSSL, [ openssl >= 0.9.8 ])
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)

DBUS_VERSION="Disabled"
DBUS_SYS_DIR="Disabled"
DBUS_SYSTEM_BUS_DEFAULT_ADDRESS="Disabled"
PKG_CHECK_MODULES(DBUS, [ dbus-1 >= 0.34 ])
DBUS_CFLAGS="$DBUS_CFLAGS -DDBUS_API_SUBJECT_TO_CHANGE"
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)
AC_SUBST(DBUS_SYS_DIR)
AC_SUBST(DBUS_SYSTEM_BUS_DEFAULT_ADDRESS)
SAVED_LIBS="$LIBS"
LIBS="$LIBS $DBUS_LIBS"
AC_CHECK_FUNCS([dbus_connection_close dbus_bus_get_private])
LIBS="$SAVED_LIBS"

if test "x$HAVE_DARWIN" = "xno"; then
    PKG_CHECK_MODULES(AVAHI_COMPAT_LIBDNS_SD, [ avahi-compat-libdns_sd >= 0.6.16 ])
    AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_CFLAGS)
    AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_LIBS)
fi

AM_CONDITIONAL(ENABLE_DARWIN, test "x$HAVE_DARWIN" = "xyes")
AM_CONDITIONAL(ENABLE_LINUX, test "x$HAVE_LINUX" = "xyes")


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
if test "x$ENABLE_QT4" = "xyes" ; then
   AC_PATH_PROG(QT4_PATH, qmake-qt4, no)
   if test "x$QT4_PATH" = xno; then
      AC_MSG_ERROR([could not find QT4 executables])
   fi
fi
AM_CONDITIONAL(ENABLE_QT4, test "x$ENABLE_QT4" = "xyes")


AC_PATH_PROG(TINYOSMIG, mig, no)
AM_CONDITIONAL([TINYOSMIG_DISABLED], [test "x$TINYOSMIG" = "xno"])
		      
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
		 client/monitor/Makefile
                 client/py-libbionet/Makefile
                 client/watcher/Makefile
                 hab/Makefile
                 hab/libhab/Makefile
		 hab/py-libhab/Makefile
                 hab/csa-cp/Makefile
		 hab/mmod/Makefile
                 hab/random/Makefile
                 data-manager/Makefile
                 data-manager/server/Makefile
                 data-manager/client/Makefile
                 ])

AC_OUTPUT
