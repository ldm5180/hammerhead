dnl @synopsis AX_EXT_HAVE_LIB(<directories>, <library>, <function>, [run-if-found], [run-if-not-found],  [extra libraries])
dnl
dnl AX_EXT_HAVE_LIB is identical to AC_SEARCH_LIBS with the exception
dnl that will add -L<directory> when looking, and use a different
dnl variable for each directory.
dnl
dnl Any required -L<directory> flags are added to LDFLAGS and located
dnl libraies are added to LIBS
dnl
dnl Some libraries are unlinkable without other extra libraries, which
dnl can be specified in the 4th argument. The mysql client library
dnl needs -lz, for example.
dnl
dnl Example:
dnl
dnl  AX_EXT_HAVE_LIB(/lib /usr/lib /usr/local/lib /usr/lib/mysql /usr/local/mysql/lib, mysqlclient, mysql_init, [-lz])
dnl
dnl which finds the mysql client library if succeds system when it
dnl tries with -L/usr/lib/mysql then it adds -lmysqlclient to LIBS and
dnl -L/usr/lib/mysql to LDFLAGS.
dnl
dnl The test itself is based on the autoconf 2.53 version of
dnl AC_SEARCH_LIBS.
dnl
dnl @category InstalledPackages
dnl @author dps@simpson.demon.co.uk
dnl @version 2004-07-29
dnl @license AllPermissive

AC_DEFUN([AX_EXT_HAVE_LIB],
[
new_ldflags=${LDFLAGS}
new_libs=$LIBS
AC_CHECK_LIB([$2], $3, new_libs="-l$2 $6"; ext_lib_found="yes",  ext_lib_found="no", $6)
for dir in $1
do
if test $ext_lib_found = no
then
ext_haslib_cvdir=`echo $dir | $as_tr_sh`
AC_CACHE_CHECK([for $2 library with -L$dir], [ext_cv${ext_haslib_cvdir}_haslib_$2],
[ext_func_search_save_LIBS=$LIBS
ext_func_save_ldflags=${LDFLAGS}
LIBS="-l$2 $6 ${ext_func_search_save_LIBS}"
LDFLAGS="-L$dir ${ext_func_save_ldflags}"
AC_TRY_LINK_FUNC([$3], [eval "ext_cv${ext_haslib_cvdir}_haslib_$2"="yes"],
[eval "ext_cv${ext_haslib_cvdir}_haslib_$2"="no"])
LIBS=$ext_func_search_save_LIBS
LDFLAGS=$ext_func_save_ldflags
])
if eval `echo 'test x${'ext_cv${ext_haslib_cvdir}_haslib_$2'}' = "xyes"`; then
new_libs="-l$2 ${new_libs}"
new_ldflags="-L${dir} ${new_ldflags}"
ext_lib_found="yes"
fi
fi
done
AS_IF([test "$ext_lib_found" = "yes"], [$4], [$5])
LIBS=$new_libs
LDFLAGS=$new_ldflags
])
