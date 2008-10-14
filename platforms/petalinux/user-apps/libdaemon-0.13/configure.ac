# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# This file is part of libdaemon.
#
# Copyright 2003-2008 Lennart Poettering
#
# libdaemon is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 2.1 of the
# License, or (at your option) any later version.
#
# libdaemon is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with libdaemon. If not, see
# <http://www.gnu.org/licenses/>.

AC_PREREQ(2.59)

AC_INIT([libdaemon],[0.13],[mzqnrzba (at) 0pointer (dot) de])
AC_CONFIG_SRCDIR([libdaemon/dfork.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign 1.9 -Wall])

AC_SUBST(PACKAGE_URL, [http://0pointer.de/lennart/projects/libdaemon/])

AC_SUBST(LIBDAEMON_VERSION_INFO, [4:0:4])

if type -p stow > /dev/null && test -d /usr/local/stow ; then
   AC_MSG_NOTICE([*** Found /usr/local/stow: default install prefix set to /usr/local/stow/${PACKAGE_NAME}-${PACKAGE_VERSION} ***])
   ac_default_prefix="/usr/local/stow/${PACKAGE_NAME}-${PACKAGE_VERSION}"
fi

# Checks for programs.
AC_PROG_CC
AC_GNU_SOURCE
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

test_gcc_flag() {
    AC_LANG_CONFTEST([int main(int argc, char*argv[]) {}])
    $CC -c conftest.c $CFLAGS "$1" -o conftest.o > /dev/null 2> /dev/null
    ret=$?
    rm -f conftest.o
    return $ret
}

# If using GCC specify some additional parameters
if test "x$GCC" = "xyes" ; then
    # We use gnu99 instead of c99 because many have interpreted the standard
    # in a way that int64_t isn't defined on non-64 bit platforms.
    DESIRED_FLAGS="-std=gnu99 -Wall -W -Wextra -pedantic -pipe -Wformat -Wold-style-definition -Wdeclaration-after-statement -Wfloat-equal -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wredundant-decls -Wmissing-noreturn -Wshadow -Wendif-labels -Wpointer-arith -Wcast-align -Wwrite-strings -Winline -Wno-unused-parameter -ffast-math"

    for flag in $DESIRED_FLAGS ; do
        AC_MSG_CHECKING([whether $CC accepts $flag])
        if test_gcc_flag $flag ; then
           CFLAGS="$CFLAGS $flag"
           CXXFLAGS="$CXXFLAGS $flag"
           AC_MSG_RESULT([yes])
        else
           AC_MSG_RESULT([no])
        fi
    done
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h syslog.h unistd.h sys/ioctl.h sys/time.h])
AC_HEADER_SYS_WAIT

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_HEADER_TIME
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([select strerror dup2 memset strrchr asprintf])
AC_TYPE_MODE_T
AC_FUNC_SETPGRP
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T

AC_SYS_LARGEFILE

# LYNX documentation generation
AC_ARG_ENABLE(lynx,
        AC_HELP_STRING(--disable-lynx,Turn off lynx usage for documentation generation),
[case "${enableval}" in
  yes) lynx=yes ;;
  no)  lynx=no ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --disable-lynx) ;;
esac],[lynx=yes])

if test x$lynx = xyes ; then
   AC_CHECK_PROG(have_lynx, lynx, yes, no)

   if test x$have_lynx = xno ; then
     AC_MSG_WARN([*** lynx not found, plain text README will not be built ***])
   fi
fi

AM_CONDITIONAL([USE_LYNX], [test "x$lynx" = xyes])

AC_CONFIG_FILES([libdaemon/Makefile Makefile doc/Makefile doc/README.html doc/doxygen.conf examples/Makefile libdaemon.pc])
AC_OUTPUT
