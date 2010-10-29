dnl
dnl $ Id: $
dnl

PHP_ARG_ENABLE(unichar, [whether to enable Unicode character functions],
[  --enable-unichar        Enable Unicode character functions], yes, yes)

PHP_ARG_WITH(unichar-icu, [whether to enable ICU support],
[  --with-unichar-icu[[=PATH]] Enable ICU-related features.
                            PATH is the optional pathname to icu-config.], no, no)

if test "$PHP_UNICHAR" != "no"; then

  if test -z "$AWK"; then
    AC_PATH_PROGS(AWK, awk gawk nawk, [no])
  fi
  if test -z "$SED"; then
    AC_PATH_PROGS(SED, sed gsed, [no])
  fi

  dnl
  dnl Check for ICU support
  dnl
  if test "$PHP_UNICHAR_ICU" != "no"; then
    dnl
    dnl Check the location of icu-config
    dnl
    if test "$PHP_UNICHAR_ICU" != "yes"; then
      AC_MSG_CHECKING([for icu-config])
      if test -f "$PHP_UNICHAR_ICU"; then
        ICU_CONFIG="$PHP_UNICHAR_ICU"
      elif test -f "$PHP_UNICHAR_ICU/icu-config"; then
        ICU_CONFIG="$PHP_UNICHAR_ICU/icu-config"
      elif test -f "$PHP_UNICHAR_ICU/bin/icu-config"; then
        ICU_CONFIG="$PHP_UNICHAR_ICU/bin/icu-config"
      else
        AC_MSG_ERROR([not found])
      fi
      AC_MSG_RESULT([$ICU_CONFIG])
    else
      AC_PATH_PROGS(ICU_CONFIG, icu-config, [])
      if test -z "$ICU_CONFIG"; then
        AC_MSG_ERROR([icu-config not found])
      fi
    fi

    dnl
    dnl Get the version number, CPPFLAGS and LDFLAGS by icu-config
    dnl
    AC_MSG_CHECKING([for ICU library version])
    ICU_VERSION=`$ICU_CONFIG --version 2> /dev/null`
    ICU_INCLINE=`$ICU_CONFIG --cppflags 2> /dev/null`
    ICU_LIBLINE=`$ICU_CONFIG --ldflags 2> /dev/null`

    if test -z "$ICU_VERSION"; then
      AC_MSG_ERROR([invalid icu-config passed to --with-unichar-icu])
    fi

    ICU_VERSION_SHORT=`echo $ICU_VERSION | $AWK -F. '{ printf "%d", $1 * 10 + $2 }'`
    ICU_VERSION_SUFFIX=`echo $ICU_VERSION | $AWK -F. '{ printf "_%d_%d", $1, + $2 }'`

    if test "$ICU_VERSION_SHORT" -lt 30; then
      AC_MSG_ERROR([ICU version 3.0 or later is required])
    fi

    AC_MSG_RESULT([$ICU_VERSION])

    dnl
    dnl Check ICU headers
    dnl
    export OLD_CPPFLAGS="$CPPFLAGS"
    export CPPFLAGS="$CPPFLAGS $INCLUDES $ICU_INCLINE"
    AC_CHECK_HEADER(unicode/uchar.h, [], AC_MSG_ERROR([unicode/uchar.h header not found]))
    AC_CHECK_HEADER(unicode/ucnv.h,  [], AC_MSG_ERROR([unicode/ucnv.h header not found]))
    AC_CHECK_HEADER(unicode/unorm.h, [], AC_MSG_ERROR([unicode/unorm.h header not found]))
    export CPPFLAGS="$OLD_CPPFLAGS"

    dnl
    dnl Check ICU library
    dnl
    export OLD_LIBS="$LIBS"
    export LIBS="$LIBS -lpthread"
    PHP_CHECK_LIBRARY(icuuc, unorm_normalize_${ICU_VERSION_SHORT},
      [
        PHP_EVAL_INCLINE($ICU_INCLINE)
        PHP_EVAL_LIBLINE($ICU_LIBLINE, UNICHAR_SHARED_LIBADD)
      ],[
        PHP_CHECK_LIBRARY(icuuc, unorm_normalize${ICU_VERSION_SUFFIX},
          [
            PHP_EVAL_INCLINE($ICU_INCLINE)
            PHP_EVAL_LIBLINE($ICU_LIBLINE, UNICHAR_SHARED_LIBADD)
          ],[
            AC_MSG_ERROR([wrong ICU library version or lib not found. Check config.log for more information])
          ],[
            $ICU_LIBLINE
          ])
      ],[
        $ICU_LIBLINE
      ])
    export LIBS="$OLD_LIBS"

    AC_DEFINE(WITH_UNICHAR_ICU, 1, [enable ICU support])
    AC_DEFINE_UNQUOTED(UNICHAR_ICU_VERSION_STRING, "$ICU_VERSION", [ICU version string])
    AC_DEFINE_UNQUOTED(UNICHAR_ICU_VERSION_NUMBER, $ICU_VERSION_SHORT, [ICU version number])
  fi

  PHP_SUBST(UNICHAR_SHARED_LIBADD)
  AC_DEFINE(HAVE_UNICHAR, 1, [enable unichar functions])

  PHP_NEW_EXTENSION(unichar, unichar.c, $ext_shared)
fi
