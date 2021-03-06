# Copyright (C) 2004-2006 Free Software Foundation, Inc.
#
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# This file represents the compiled summary of the specification in
# gnulib-cache.m4. It lists the computed macro invocations that need
# to be invoked from configure.ac.
# In projects using CVS, this file can be treated like other built files.


# This macro should be invoked from ./configure.ac, in the section
# "Checks for programs", right after AC_PROG_CC, and certainly before
# any checks for libraries, header files, types and library functions.
AC_DEFUN([gl_EARLY],
[
  AC_REQUIRE([AC_PROG_RANLIB])
  AC_REQUIRE([AC_GNU_SOURCE])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
])

# This macro should be invoked from ./configure.ac, in the section
# "Check for header files, types and library functions".
AC_DEFUN([gl_INIT],
[
  AM_CONDITIONAL([GL_COND_LIBTOOL], [true])
  gl_FUNC_ALLOCA
  gl_ALLOCSA
  gl_ERROR
  gl_EXITFAIL
  dnl gl_USE_SYSTEM_EXTENSIONS must be added quite early to configure.ac.
  gl_GETDATE
  gl_GETTIME
  AC_FUNC_GETTIMEOFDAY_CLOBBER
  gl_FUNC_MKTIME
  gl_C_RESTRICT
  gt_FUNC_SETENV
  AM_STDBOOL_H
  gl_FUNC_GNU_STRFTIME
  gl_TIME_R
  gl_TIMESPEC
  gl_XALLOC
])

# This macro records the list of files which have been installed by
# gnulib-tool and may be removed by future gnulib-tool invocations.
AC_DEFUN([gl_FILE_LIST], [
  doc/getdate.texi
  lib/alloca_.h
  lib/allocsa.c
  lib/allocsa.h
  lib/allocsa.valgrind
  lib/error.c
  lib/error.h
  lib/exit.h
  lib/exitfail.c
  lib/exitfail.h
  lib/getdate.h
  lib/getdate.y
  lib/gettext.h
  lib/gettime.c
  lib/gettimeofday.c
  lib/mktime.c
  lib/setenv.c
  lib/setenv.h
  lib/stdbool_.h
  lib/strftime.c
  lib/strftime.h
  lib/time_r.c
  lib/time_r.h
  lib/timespec.h
  lib/unsetenv.c
  lib/xalloc-die.c
  lib/xalloc.h
  lib/xmalloc.c
  m4/alloca.m4
  m4/allocsa.m4
  m4/bison.m4
  m4/clock_time.m4
  m4/eealloc.m4
  m4/error.m4
  m4/exitfail.m4
  m4/extensions.m4
  m4/getdate.m4
  m4/gettime.m4
  m4/gettimeofday.m4
  m4/longdouble.m4
  m4/longlong.m4
  m4/mbstate_t.m4
  m4/mktime.m4
  m4/onceonly_2_57.m4
  m4/restrict.m4
  m4/setenv.m4
  m4/stdbool.m4
  m4/strerror_r.m4
  m4/strftime.m4
  m4/time_r.m4
  m4/timespec.m4
  m4/tm_gmtoff.m4
  m4/xalloc.m4
])
