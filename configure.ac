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

#PKG_CHECK_MODULES(AVAHI_GLIB, [ avahi-glib >= 0.6.16 ])
#AC_SUBST(AVAHI_GLIB_CFLAGS)
#AC_SUBST(AVAHI_GLIB_LIBS)

PKG_CHECK_MODULES(AVAHI_COMPAT_LIBDNS_SD, [ avahi-compat-libdns_sd >= 0.6.16 ])
AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_CFLAGS)
AC_SUBST(AVAHI_COMPAT_LIBDNS_SD_LIBS)

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
                 client/watcher/Makefile
                 hab/Makefile
                 hab/libhab/Makefile
                 hab/random/Makefile
                 ])

AC_OUTPUT
