
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(SMACQ, 2.1, mfisk@lanl.gov)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(1.7)
AM_PROG_CC_STDC
AC_CONFIG_SRCDIR([bin])
#AC_CONFIG_HEADERS()
AC_LIBTOOL_DLOPEN

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_ARG_VAR(PYTHON, [Python 2.3 interpreter command])
AC_CHECK_PROGS([PYTHON], [python2.3 python], :)

# Checks for libraries.
AC_CHECK_LIB([c], [printf])
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([fl], [yylex])
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

PKG_CHECK_MODULES(DEPS, glib-2.0 gmodule-2.0)

LDADDS=${DEPS_LIBS}
GLOBAL_INCLUDES="${DEPS_CFLAGS} -D_GNU_SOURCE -D_ISOC99_SOURCE"
AC_SUBST(GLOBAL_INCLUDES)
AC_SUBST(LDADDS)

SHAREDONLY_MODULES="test_dynamic" #reassemble
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

PKG_CHECK_MODULES(GDA, libgda,
	[LDFLAGS=${GDA_LIBS}
	GLOBAL_INCLUDES="${GDA_CFLAGS} ${GLOBAL_INCLUDES}"
	SQLINSERT_LIBS="${EXTRA_MODULE_SOURCES} sqlinsert.lo"
	OPTIONAL_TESTS="${OPTIONAL_TESTS} sqlinsert.test"
	],[echo "$GDA_PKG_ERRORS"
	echo "Building without sqlinsert module"])

AC_SUBST(SQLINSERT_LIBADD, $SQLINSERT_LIBS)
AC_SUBST(SQLINSERT_DEPENDENCIES, $SQLINSERT_LIBS)
AC_SUBST(OPTIONAL_TESTS)

AC_ARG_ENABLE(profile,
	AS_HELP_STRING([--enable-profile],
			[enable gprof profiling]),
		ADDCFLAGS="$ADDCFLAGS -pg"
		LDFLAGS="$LDFLAGS -pg"
	],)

AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],
			[enable debugging options]),
	[if test -n "$GCC"; then
		CO="-O0"
		ADDCFLAGS="$ADDCFLAGS -fno-inline -g -Wall -Werror"
	else
		CO="-O0"
		ADDCFLAGS="$ADDCFLAGS -g"
	fi],
	[if test -n "$GCC"; then
		ADDCFLAGS="$ADDCFLAGS -g" # -fast on MacOS
	fi
	CO="-O9"
	])

AC_ARG_ENABLE(efence,
	AS_HELP_STRING([--enable-efence],
			[Enable electric-fence memory debugging]),
		[LDFLAGS="$LDFLAGS -lefence"])

AC_ARG_WITH([ethereal], 
	AS_HELP_STRING([--with-ethereal],
			[Include Ethereal packet decoding]),
	[with_ethereal="$withval"],
	[with_ethereal=no])


AC_SUBST(ETHEREAL_LIBRARY)
AC_SUBST(ETHEREAL_DIR)


##
## Python
##

AC_ARG_WITH([python],
	AS_HELP_STRING([--with-python], 
			[Include Python (default=use if available)]),
	[with_python="$withval"],
	[with_python=auto])

if test -n "$GCC"; then
	ADDCFLAGS="$ADDCFLAGS -Wall"
fi

if test -z "$CO"; then
 	CO="-O9"
fi

CFLAGS="$CFLAGS $ADDCFLAGS $CO"
CXXFLAGS="$CXXFLAGS $ADDCFLAGS $CO"

if test "X$with_ethereal" = "Xno"; then
	ETHEREAL_LIBRARY=
	ETHEREAL_DIR=
else
	ETHEREAL_LIBRARY=libethereal.la
	ETHEREAL_DIR=${with_ethereal}
	CFLAGS="$CFLAGS -DWITH_ETHEREAL -I$ETHEREAL_DIR"
fi

# Try to detect whether or not to build with python
if test "X$with_python" = "Xauto"; then
	if test "X$PYTHON" = "X:"; then
		AC_MSG_NOTICE([Building without Python (use PYTHON=/path/to/python2.3 to include it])
		with_python=no
	else
		with_python=yes
	fi
fi

if test "X$with_python" = "Xyes"; then
	PYCONF=ac_config.py
	cat >$PYCONF <<EOF
[from distutils import sysconfig
import sys

assert sys.version_info > (2, 3, 0)

res = []
for i in sys.argv[1:]:
    if i == 'CFLAGS':
        res.append('-I %s' % sysconfig.get_python_inc())
    else:
        res.append(sysconfig.get_config_var(i))
print ' '.join(res)
]
EOF

	AC_MSG_CHECKING([CFLAGS_PYTHON])
	CFLAGS_PYTHON=`$PYTHON $PYCONF CFLAGS 2>&5`
	if test $? -ne 0; then
		rm -f $PYCONF
		AC_MSG_FAILURE([Python config script failed])
	fi
	AC_MSG_RESULT($CFLAGS_PYTHON)

	AC_MSG_CHECKING([LDFLAGS_PYTHON])
	LDFLAGS_PYTHON=`$PYTHON $PYCONF LINKFORSHARED LIBS BLDLIBRARY`
	if test $? -ne 0; then
		rm -f $PYCONF
		AC_MSG_FAILURE([Python config script failed])
	fi
	AC_MSG_RESULT($LDFLAGS_PYTHON)

	AC_MSG_CHECKING([LIBS_PYTHON])
	LIBS_PYTHON=`$PYTHON $PYCONF LIBS BLDLIBRARY`
	if test $? -ne 0; then
		rm -f $PYCONF
		AC_MSG_FAILURE([Python config script failed])
	fi
	AC_MSG_RESULT($LIBS_PYTHON)

	PYTHON_LIBADD="$LDFLAGS_PYTHON python.lo"
	PYTHON_DEPENDENCIES="python.lo"
	PYTHON_TESTS="python.test"

	rm -f $PYCONF
fi
AC_SUBST(PYTHON)
AC_SUBST(CFLAGS_PYTHON)
AC_SUBST(LDFLAGS_PYTHON)
AC_SUBST(LIBS_PYTHON)
AC_SUBST(PYTHON_LIBADD)
AC_SUBST(PYTHON_DEPENDENCIES)
AC_SUBST(PYTHON_TESTS)

AM_CONDITIONAL(USE_PYTHON, test x$with_python = xyes)


##
##
##

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 bin/Makefile
                 doc/Makefile
                 libsmacq/Makefile
                 misc/Makefile
                 modules/Makefile
                 modules/substr/Makefile
                 modules/patricia/Makefile
                 test/Makefile
                 types/Makefile])
AC_OUTPUT

