#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ(2.57)
AC_INIT(SMACQ, , mfisk@lanl.gov)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(SMACQ,)
AM_PROG_CC_STDC
AC_CONFIG_SRCDIR([bin])
AC_CONFIG_HEADERS

AC_LIBTOOL_DLOPEN

# Checks for programs.
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lc':
AC_CHECK_LIB([c], [main])
AC_CHECK_LIB([dl], [dlopen])
# FIXME: Replace `main' with a function in `-lfl':
AC_CHECK_LIB([fl], [main])
AC_CHECK_LIB([glib], [g_new])
AC_CHECK_LIB([m], [pow])
AC_CHECK_LIB([pcap], [pcap_open_live])
AC_CHECK_LIB([pthread], [pthread_exit])
# FIXME: Replace `main' with a function in `-lutil':
AC_CHECK_LIB([util], [main])
AC_CHECK_LIB([z], [gzopen])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h inttypes.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor gethostbyname gettimeofday inet_ntoa localtime_r memmove memset pow select socket strchr strdup strerror strtol])

GLOBAL_INCLUDES="-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -D_GNU_SOURCE -D_ISOC99_SOURCE"
AC_SUBST(GLOBAL_INCLUDES)

SHAREDONLY_MODULES="test_dynamic" #python #reassemble
BUILTIN_MODULES="print pcaplive pcapfile socket uniq count last top head filter msgtest entropy derivative split encrypt warden pdf fifo delta tabularinput sync rusage groupby flowid sum fifodelay project rename join substr bench_field mask const ndjoin lor counter_loop dfa noop expr equals cflow clock"
BUILTIN_TYPES="int ushort ubyte timeval uint32 bytes netlong ip nushort nuint32 macaddr string double refresh empty ipproto packet cflow time ntime"

SOURCES_TYPES_BUILTIN=`for f in $BUILTIN_TYPES; do echo -n " $f.c "; done`
AC_SUBST(SOURCES_TYPES_BUILTIN)

LIBS_MODULES_ALL=`for f in $BUILTIN_MODULES $SHAREDONLY_MODULES; do echo -n " lib$f.la "; done`
AC_SUBST(LIBS_MODULES_ALL)

SOURCES_MODULES_BUILTIN=`for f in $BUILTIN_MODULES; do echo -n " $f.c "; done`
AC_SUBST(SOURCES_MODULES_BUILTIN)

OBJECTS_MODULES_BUILTIN=`for f in $BUILTIN_MODULES; do echo -n " lib$f.la "; done`
AC_SUBST(OBJECTS_MODULES_BUILTIN)

LIBS_MODULES_SHARED=`for f in $SHAREDONLY_MODULES; do echo -n " smacq_$f.la "; done`
AC_SUBST(LIBS_MODULES_SHARED)

if test -n "$GCC"; then
	CFLAGS="$CFLAGS -Wall -O9"
fi

AC_ARG_ENABLE(profile,
	AC_HELP_STRING([--enable-profile],[enable gprof profiling]),
		CFLAGS="$CFLAGS -pg"
		LDFLAGS="$LDFLAGS -pg"
	fi],)

AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug],[enable debugging options]),
	[if test -n "$GCC"; then
		CFLAGS="$CFLAGS -O0 -ggdb"
	else
		CFLAGS="$CFLAGS -O0 -g"
	fi],)

AC_ARG_WITH([ethereal], 
	AC_HELP_STRING([--with-ethereal],[Include Ethereal packet decoding]),
	[with_ethereal="$withval"],
	[with_ethereal=no])

if test "$with_ethereal" = "no"; then
	ETHEREAL_LIBRARY=
	ETHEREAL_DIR=
else
	ETHEREAL_LIBRARY=libethereal.la
	ETHEREAL_DIR=${with_ethereal}
	CFLAGS="$CFLAGS -DWITH_ETHEREAL -I$ETHEREAL_DIR"
fi
AC_SUBST(ETHEREAL_LIBRARY)
AC_SUBST(ETHEREAL_DIR)

AC_CONFIG_FILES([Makefile
                 bin/Makefile
                 doc/Makefile
                 libsmacq/Makefile
                 misc/Makefile
                 modules/Makefile
                 modules/substr/Makefile
                 test/Makefile
                 types/Makefile])
AC_OUTPUT
