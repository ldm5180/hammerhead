
# AC_FIND_QWT(QWT-VERSION, QT-VERSION, SEARCH-LIBS,
#             INCLUDE-SEARCH-PATH, [LIBRARY-SEARCH-PATH], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------
# Search for qwt libraries and headers.
# DEFINES QWT_LIBADD QWT_LDFLAGS QWT_CFLAGS
# Fails if not found
AC_DEFUN([AC_FIND_QWT],
[AC_LANG_PUSH(C++)
 hdr=`echo qwt.h | $as_tr_sh`
 got=no
 xtra_incdir="-I/usr/include/qt4 -I/usr/include/qt4/Qt"
 if test "x$1" != "x"; then
  minqwt=`echo $1 | awk -F . '{printf "0x%02d%02d%02d", $[]1, $[]2, $[]3}'`
 else
  minqwt=0x050000
 fi
 for dir in $4; do
  if test "x${got}" = "xno"; then
   ext_cflag_cvdir=`echo $dir | $as_tr_sh`
   AC_CACHE_CHECK([for qwt.h header with -I$dir],
    [ext_cv${ext_cflag_cvdir}_cflag_${hdr}],
    [ext_have_hdr_save_cppflags=${CFLAGS}
     ext_have_hdr_save_cflags=${CPPFLAGS}
     CFLAGS="${xtra_incdir} -I${dir} ${CFLAGS}"
     CPPFLAGS="${xtra_incdir} -I${dir} ${CPPFLAGS}"
     AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
                [#include <qwt.h>],
                [[#if QWT_VERSION < $minqwt
                  #error Old QWT Version
                  #endif]]
        )],
       [got="yes"; eval "ext_cv${ext_cflag_cvdir}_cflag_${hdr}"="yes"],
       [got="no"; eval "ext_cv${ext_cflag_cvdir}_cflag_${hdr}"="no"])
      CFLAGS=$ext_have_hdr_save_cflags
      CPPFLAGS=$ext_have_hdr_save_cppflags
      ])
     if test "x${got}" = "xyes"; then
      AC_SUBST([CFLAGS_QWT], [-I${dir}])
      AC_SUBST([INCLUDE_DIRS_QWT], [$dir])
     fi;
  fi; 
 done

 for dir in '' $5; do 
  if test "x${got}" = "xno"; then
   ext_ldflag_cvdir=`echo $dir | $as_tr_sh`
   AS_VAR_PUSHDEF([ac_cv_qwt], [ac_cv_qwt_${ext_ldflag_cvdir}lib])dnl
   AC_CACHE_CHECK([for qwt libs], [ac_cv_qwt],
    [ac_func_search_save_LIBS=$LIBS
     ac_func_search_save_LDFLAGS=$LDFLAGS
    AC_LANG_CONFTEST([AC_LANG_CALL([], [_ZN7QwtPlot8initPlotERK7QwtText])])
    for ac_lib in '' $3; do
      if test -z "$ac_lib"; then
        ac_res=""
      else
        ac_res=-l$ac_lib
        LIBS="-l$ac_lib $ac_func_search_save_LIBS"
      fi
      AC_LINK_IFELSE([], [AS_VAR_SET([ac_cv_qwt], [$ac_res])])
      AS_VAR_SET_IF([ac_cv_qwt], [break])dnl
    done
    AS_VAR_SET_IF([ac_cv_qwt], , [AS_VAR_SET([ac_cv_qwt], [no])])dnl
    rm conftest.$ac_ext
    LIBS=$ac_func_search_save_LIBS])
   ac_res=AS_VAR_GET([ac_cv_qwt])
   AS_IF([test "$ac_res" != no],
     [got=yes; 
      AC_SUBST([QWT_LIBADD], ["$ac_res"])
      AC_SUBST([QWT_LDFLAGS], ["-L${dir}"])
     ])
   AS_VAR_POPDEF([ac_cv_qwt])
  fi;
 done;
 if test "x$got" = "xno"; then
   AC_MSG_WARN([Unable to find qwt libs or headers])
   [$6]
 fi;
AC_LANG_POP(C++)
])
