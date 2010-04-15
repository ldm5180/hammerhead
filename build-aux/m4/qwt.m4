# _AC_MSG_LOG_FILE
# --------------------
m4_define([_AC_MSG_LOG_FILE],
[echo "$as_me: failed text was:" >&AS_MESSAGE_LOG_FD
sed 's/^/| /' $1 >&AS_MESSAGE_LOG_FD
])


# AC_QMAKE_COMPILE_IFELSE(PROGRAM, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# --------------------------------------------------------
# Try to compile the program using qmake.
AC_DEFUN([AC_QMAKE_COMPILE_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest.$ac_objext conftest.mk
[cat >conftest.pro <<_ACEOF
SOURCES += conftest.$ac_ext
INCLUDEPATH += $QTINCDIRS
_ACEOF]

AS_IF([_AC_DO_STDERR([$QMAKE -o conftest.mk conftest.pro]) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s conftest.pro],
      [AS_IF([_AC_DO_STDERR([make -f conftest.mk conftest.$ac_objext 1>&AS_MESSAGE_LOG_FD]) && {
               test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
               test ! -s conftest.err
             } && test -s conftest.$ac_objext],
             [$2],
             [_AC_MSG_LOG_CONFTEST
               $3])
      ],
      [_AC_MSG_LOG_FILE(conftest.pro)
        $3])
rm -f core conftest.pro conftest.mk conftest.err conftest.$ac_objext m4_ifval([$1], [conftest.$ac_ext])[]dnl
])

# AC_QMAKE_LINK_IFELSE(PROGRAM, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# --------------------------------------------------------
# Try to compile and link the program using qmake.
AC_DEFUN([AC_QMAKE_LINK_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest.$ac_objext conftest.mk
[cat >conftest.pro <<_ACEOF
SOURCES += conftest.$ac_ext
LIBS += $QTLDFLAGS $QTLDADD
_ACEOF]

AS_IF([_AC_DO_STDERR([$QMAKE -o conftest.mk conftest.pro]) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s conftest.pro],
      [AS_IF([_AC_DO_STDERR([make -f conftest.mk 1>&AS_MESSAGE_LOG_FD]) && {
               test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
               test ! -s conftest.err
             }],
             [$2],
             [_AC_MSG_LOG_CONFTEST
               $3])
      ],
      [_AC_MSG_LOG_FILE(conftest.pro)
        $3])
rm -f core conftest.pro conftest.mk conftest.err conftest.$ac_objext m4_ifval([$1], [conftest.$ac_ext])[]dnl
])


# AC_FIND_QWT(QWT-VERSION, QT-VERSION, SEARCH-LIBS,
#             INCLUDE-SEARCH-PATH, [LIBRARY-SEARCH-PATH], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------
# Search for qwt libraries and headers.
# DEFINES QWT_LIBADD QWT_LDFLAGS QWT_CFLAGS
# For each requiremnet not met (headers, libs), ACTION-IF-NOT-FOUND is run
AC_DEFUN([AC_FIND_QWT],
[AC_LANG_PUSH(C++)
 hdr=`echo qwt.h | $as_tr_sh`
 got=no
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
    [ext_qwt_hdr_save_INCDIR=${QTINCDIRS}
     QTINCDIRS="${dir} ${QTINCDIRS}"
     AC_QMAKE_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
                [#include <qwt.h>],
                [[#if QWT_VERSION < $minqwt
                  #error Old QWT Version
                  #endif]]
        )],
       [got="yes"; eval "ext_cv${ext_cflag_cvdir}_cflag_${hdr}"="yes"],
       [got="no"; eval "ext_cv${ext_cflag_cvdir}_cflag_${hdr}"="no"])
      QTINCDIRS=$ext_qwt_hdr_save_INCDIR
      ])
     if test "x${got}" = "xyes"; then
      AC_SUBST([CFLAGS_QWT], [-I${dir}])
      AC_SUBST([INCLUDE_DIRS_QWT], [$dir])
     fi;
  fi; 
 done
 if test "x$got" = "xno"; then
   AC_MSG_WARN([Unable to find qwt headers])
   [$6]
 fi;

 got=no
 for dir in '' $5; do 
  if test "x${got}" = "xno"; then
   ext_ldflag_cvdir=`echo $dir | $as_tr_sh`
   AS_VAR_PUSHDEF([ac_cv_qwt], [ac_cv_qwt_${ext_ldflag_cvdir}lib])dnl
   AC_CACHE_CHECK([for qwt libs with -L${dir}], [ac_cv_qwt],
    [ac_func_qwt_save_LIBS=$QTLDADD
     ac_func_qwt_save_LDFLAGS=$QTLDFLAGS
    AC_LANG_CONFTEST([AC_LANG_CALL([], [_ZN7QwtPlot8initPlotERK7QwtText])])
    for ac_lib in '' $3; do
      if test -n "${dir}"; then
        QTLDFLAGS="-L${dir} $ac_func_qwt_save_LDFLAGS"
      fi
      if test -z "$ac_lib"; then
        tmpqwtlib=""
      else
        tmpqwtlib=-l$ac_lib
        QTLDADD="-l$ac_lib $ac_func_qwt_save_LIBS"
      fi
      AC_QMAKE_LINK_IFELSE([], [AS_VAR_SET([ac_cv_qwt], [$tmpqwtlib])])
      AS_VAR_SET_IF([ac_cv_qwt], [break])
    done
    AS_VAR_SET_IF([ac_cv_qwt], , [AS_VAR_SET([ac_cv_qwt], [no])])
    rm conftest.$ac_ext
    QTLDADD=$ac_func_qwt_save_LIBS])
   AS_IF([test "x$ac_res" != "xno"],
     [got=yes; 
      AC_SUBST([QWT_LIBADD], ["$ac_res"])
      if test "x${dir}" != "x"; then
       AC_SUBST([QWT_LDFLAGS], ["-L${dir}"])
      fi
     ])
   AS_VAR_POPDEF([ac_cv_qwt])
  fi;
 done;
 if test "x$got" = "xno"; then
   AC_MSG_WARN([Unable to find qwt libs])
   [$6]
 fi;
AC_LANG_POP(C++)
])


