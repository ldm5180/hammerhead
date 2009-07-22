dnl @synopsis AX_EXT_CHECK_HEADER(<header>, <paths>, [run-if-found], [run-if-not-found], [includes = `AC_INCLUDES_DEFAULT']).
dnl
dnl Check for <header> with -I<path> for each path in <paths> if need
dnl be. The first sucecssful path is chosen (eg if you say
dnl AX_EXT_CHECK_HEADER(foo.h, bar baz qux) and -Ibaz works then -Iqux
dnl will not be tested.
dnl
dnl Any -I flags that locate a header are added to CFLAGS and CPPFLAGS.
dnl AS with AC_CHECK_HEADERS it causes HAVE_<header>_H to be defined as
dnl 1.
dnl
dnl Example:
dnl
dnl  AX_EXT_HAVE_HEADER(openssl/rsa.h, /usr/local/include /usr/local/ssl/include /usr/local/openssl/include)
dnl
dnl @category InstalledPackages
dnl @author Duncan Simpson <dps@simpson.demon.co.uk>
dnl @version 2005-01-21
dnl @license AllPermissive

AC_DEFUN([AX_EXT_HAVE_HEADER],
[AC_LANG_PUSH(C)
 AC_CHECK_HEADERS($1, [got="yes"], [got="no"], $5)
 hdr=`echo $1 | $as_tr_sh`
 for dir in $2; do
  if test "x${got}" = "xno"; then
   ext_hashdr_cvdir=`echo $dir | $as_tr_sh`
   AC_CACHE_CHECK([for $1 library with -I$dir],
    [ext_cv${ext_hashdr_cvdir}_hashdr_${hdr}],
    [ext_have_hdr_save_cflags=${CFLAGS}
     CFLAGS="${CFLAGS} -I${dir}"
     AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM([#include <$1>])],
       [got="yes"; eval "ext_cv${ext_hashdr_cvdir}_hashdr_${hdr}"="yes"],
       [got="no"; eval "ext_cv${ext_hashdr_cvdir}_hashdr_${hdr}"="no"])
      CFLAGS=$ext_have_hdr_save_cflags])
     if eval `echo 'test x${'ext_cv${ext_hashdr_cvdir}_hashdr_${hdr}'}' = "xyes"`; then
      CFLAGS="${CFLAGS} -I${dir}"
      CPPFLAGS="${CPPFLAGS} -I${dir}"
      got="yes";
      hdr=`echo $1 | $as_tr_cpp`
      AC_DEFINE_UNQUOTED(HAVE_${hdr}, 1,
       [Define this if you have the $1 header])
  fi; fi; done
  AS_IF([test "x${got}" = "xyes"], [$3], [$4])
AC_LANG_POP])
