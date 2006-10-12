# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(SMACQ, 2.5, mfisk@lanl.gov)
AC_CONFIG_AUX_DIR([config])
AM_CONFIG_HEADER(include/config.h)
AM_INIT_AUTOMAKE(1.7)
AC_CONFIG_SRCDIR([bin])


##
## Checks for programs
##
AC_PROG_CC
gl_EARLY # gnulib wants to start early
AM_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_YACC
AC_PROG_LN_S
AC_PROG_MAKE_SET

##
## Use Flex
##
AM_PROG_LEX
AM_CPPFLAGS="$AM_CPPFLAGS -Wno-deprecated" #Flex uses iostream.h
AC_SUBST(LEX_OUTPUT_ROOT, lex.Smacq)

##
## Standard libraries
##
AC_CHECK_LIB([c], [printf])
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([fl], [yylex])
AC_CHECK_LIB([glib-2.0], [g_new]) 
AC_CHECK_LIB([m], [pow])
AC_CHECK_LIB([pcap], [pcap_open_live])
AC_CHECK_LIB([pthread], [pthread_exit])
AC_CHECK_LIB([z], [gzopen])
AC_CHECK_LIB([bz2], [BZ2_bzopen])

##
## Libtool
##
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
#LT_INIT([dlopen])

##
## Platform specific compiler flag defaults
## (this is done early so they can be overridden later)
##
if test -n "$GCC"; then
	case $host in  #This breaks cross-compiling
		sparc-sun-solaris*)
			CO="-fast"
			;;
		*-apple-darwin*)
			AM_CPPFLAGS="$AM_CPPFLAGS -force_flat_namespace"
			LIBGNU_CFLAGS="-Wno-long-double"
			CO="-fast -fPIC -mcpu="`machine | sed 's/ppc//'`
			DEBUG_LDFLAGS="-lMallocDebug"

			# This became necessary when I switched from Fink to DarwinPorts (on 10.4 Intel):
			AM_LDFLAGS="$AM_LDFLAGS -framework CoreFoundation"
			;;
    esac
fi
AC_SUBST(LIBGNU_CFLAGS)

##
## Support DarwinPorts automatically
##
if test -d /opt/local/include ; then 
	AM_CPPFLAGS="$AM_CPPFLAGS -I/opt/local/include"
fi

##
## pkg-config libraries
##
PKG_CHECK_MODULES(GLIB, glib-2.0)
	# Pretty much everybody needs the include files, thanks to smacq.h,
	# so don't make it specific to targets that use GLIB_CFLAGS
AM_CPPFLAGS="$AM_CPPFLAGS ${GLIB_CFLAGS}"
GLIB_CFLAGS=
AC_SUBST(GLIB_CFLAGS GLIB_LIBS)

##
## Checks for header files.
##
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h inttypes.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])
	# Thank you Red Hat (version 8 or something):
AC_CHECK_HEADER(pcap/pcap.h, AM_CPPFLAGS="$AM_CPPFLAGS -I/usr/include/pcap")

##
## Checks for typedefs, structures, and compiler characteristics.
##
AC_C_CONST
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

##
## Checks for library functions.
##
# These 4 cause gnulib to look for extra files on Linux.  ???
#AC_FUNC_MALLOC
#AC_FUNC_MEMCMP
#AC_FUNC_REALLOC
#AC_FUNC_STRTOD
AC_FUNC_MMAP
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([floor gethostbyname gettimeofday inet_ntoa localtime_r memmove memset pow select socket strchr strdup strerror strtol malloc memcmp realloc strtod clock_gettime inet_pton])

##
## Defines for makefiles
##
MAN1_MANS="\$(top_srcdir)/doc/smacq.1 \$(top_srcdir)/doc/smacqq.1"
#MAN3_MANS="\$(top_srcdir)/doc/dts-modules.3 \$(top_srcdir)/doc/api/man3/[^_]*.3"
DOCS="smacq.1.pdf smacqq.1.pdf index.html gotsmacq.png icon.png api/refman.pdf api/html/*"
AC_SUBST(MAN1_MANS)
AC_SUBST(MAN3_MANS)
AC_SUBST(DOCS)

##
## module manifest
##
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

##
## Checks can add things to these variables
##
AC_SUBST(EXTRA_STATIC_MODULES)
AC_SUBST(OPTIONAL_TESTS)

##
## --with-docdir
##
AC_DEFUN([WITH_DOCDIR],
[AC_ARG_WITH(docdir,
            AC_HELP_STRING([--with-docdir=DIR],
                           [Use DIR to store documentation files (default ${datadir}/doc/smacq)]),
            [with_docdir=$withval])
if test "x$with_docdir" = "x" ; then
  docdir='${datadir}/doc/smacq'
else
  docdir=$with_docdir
fi
AC_SUBST(docdir)
])

dnl Run the check for --with-docdir
WITH_DOCDIR

##
## libgda
##
PKG_CHECK_MODULES(GDA, libgda-2.0,
  [EXTRA_STATIC_MODULES="${EXTRA_STATIC_MODULES} libsmacq_sql.la"
  OPTIONAL_TESTS="${OPTIONAL_TESTS} sqlite.test.sh"
  ],[
  echo "$GDA_PKG_ERRORS"
  PKG_CHECK_MODULES(GDA, libgda,
    [
      EXTRA_STATIC_MODULES="${EXTRA_STATIC_MODULES} libsmacq_sql.la"
      OPTIONAL_TESTS="${OPTIONAL_TESTS} sqlite.test.sh"
      OPTIONAL_TESTS="${OPTIONAL_TESTS} sqlite.test.sh"
      GDA_VERSION=103000
    ],[
      echo "$GDA_PKG_ERRORS"
      echo "Building without modules: sqlinsert sqlquery"
    ])
  ])

##
## Profiling
##
AC_ARG_ENABLE(profile,
	AC_HELP_STRING([--enable-profile],
			[enable gprof profiling]),
		AM_CPPFLAGS="$AM_CPPFLAGS -pg"
		AM_LDFLAGS="$AM_LDFLAGS -pg"
	],)

##
## Debug
##	
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug],
			[enable debugging options]),
	[
	 AM_CPPFLAGS="$AM_CPPFLAGS -D_GLIBCXX_DEBUG";
	 if test -n "$GCC"; then
		CO="-O0"
		# Hard to debug when everything is inlined
		AM_CPPFLAGS="$AM_CPPFLAGS -g -fno-inline"
		AM_LDFLAGS="$AM_LDFLAGS ${DEBUG_LDFLAGS}"
	 else
		CO="-O0"
		AM_CPPFLAGS="$AM_CPPFLAGS -g"
	 fi
	],
        [CO="-O9"])

##
## Small (-Os)
##	
AC_ARG_ENABLE(small,
	AC_HELP_STRING([--enable-small],
			[compile with -Os to minize executable size]),
	[if test -n "$GCC"; then
		CO="-Os"
	fi])

##
## Electric Fence
##
AC_ARG_ENABLE(efence,
	AC_HELP_STRING([--enable-efence],
		       [Enable electric-fence memory debugging]))

if test "X$with_efence" != "X"; then
   AM_LDFLAGS="$AM_LDFLAGS -lefence";
fi

##
## GASNet/UPC
##
AC_ARG_WITH([upc], 
	AC_HELP_STRING([--with-upc],
			[Build parallel smacq using UPC]))

gasnet_config() {
make -s -f - <<EOM
all: force
	@echo \$($2)

include $1

force:
EOM
}

if test "X$with_upc" != "X"; then
	if test "X$with_upc" = "yes"; then
		AC_MSG_ERROR([Must specify path to upc installation with --with-upc=])
	fi
	network="smp"
	platform=${network}-seq
	mak=$withval/include/smp-conduit/${platform}.mak
	UPCC=$withval/bin/upcc
	
	AC_MSG_CHECKING([GASNET_CPPFLAGS])
	#GASNET_CPPFLAGS=-I`$UPCC -print-include-dir`
	GASNET_CPPFLAGS=`gasnet_config $mak GASNET_CPPFLAGS`
	AC_MSG_RESULT($GASNET_CPPFLAGS) 
	
	AC_MSG_CHECKING([GASNET_LIBS])
	upcc_opts=`$UPCC -network=$network --version`
	GASNET_LIBS=`for word in $upcc_opts; do case $word in -l*|-L*) echo -n "$word ";; esac; done`
	#GASNET_LIBS=`gasnet_config $mak GASNET_LIBS`
	AC_MSG_RESULT($GASNET_LIBS)

	AM_CPPFLAGS="$AM_CPPFLAGS -DUSE_GASNET $GASNET_CPPFLAGS"
	AM_LDFLAGS="$AM_LDFLAGS $GASNET_LIBS"
	EXTRA_STATIC_MODULES="${EXTRA_STATIC_MODULES} libsmacq_distribute.la"
fi

AM_CONDITIONAL(GASNET, [test "$GASNET_CPPFLAGS" != ""])

##
## Ethereal
##
AC_ARG_WITH([ethereal], 
	AC_HELP_STRING([--with-ethereal],
			[Include Ethereal packet decoding]))

if test "X$with_ethereal" != "X"; then
	if test "X$with_ethereal" = "Xyes"; then
		AC_MSG_ERROR([Must specify path to source with --with-ethereal=])
	fi
	ETHEREAL_LDFLAGS=$withval/epan/libethereal.la -lsnmp -lcrypto
	ETHEREAL_CPPFLAGS="-DWITH_ETHEREAL=$withval -I$withval"
fi

AC_SUBST(ETHEREAL_LDFLAGS)
AC_SUBST(ETHEREAL_CPPFLAGS)

##
## LBL FastBit
##
AC_ARG_WITH([fastbit], 
	AC_HELP_STRING([--with-fastbit],
			[Include FastBit indexing]))

if test "X$with_fastbit" != "X"; then
	CFLAGS_FASTBIT="-I$withval -I$withval/../stork/ibis"
	LDFLAGS_FASTBIT="-L$withval -lfastbit"
	EXTRA_STATIC_MODULES="${EXTRA_STATIC_MODULES} libsmacq_fastbit.la"
fi

AC_SUBST(CFLAGS_FASTBIT)
AC_SUBST(LDFLAGS_FASTBIT)

##
## Python
##
AC_ARG_VAR(PYTHON, [Python interpreter command])
AC_CHECK_PROGS([PYTHON], [python], :)

AC_ARG_WITH([python],
	AC_HELP_STRING([--with-python], 
			[Include Python (default=use if available)]),
	[],
	[
		# Try to auto detect whether or not to build with python
		if test "X$PYTHON" = "X:"; then
			AC_MSG_NOTICE([Building without Python; use PYTHON=/path/to/python to include it])
			with_python=no
		fi
	])

if test "X$with_python" != "Xno"; then
	PYCONF=ac_config.py
	cat >$PYCONF <<EOF
[from distutils import sysconfig
import sys

assert sys.version_info >= (2, 2, 0)

res = []
for i in sys.argv[1:]:
    if i == 'CFLAGS':
        res.append('-I %s' % sysconfig.get_python_inc())
    elif i == 'PYTHON_LIB':
        res.append(sysconfig.get_python_lib())
    elif i == 'VERSION':
        res.append(sys.version[:3])
    else:
        res.append(sysconfig.get_config_var(i))
print ' '.join(res)
]
EOF
fi

if test "X$with_python" != "Xno"; then
	CFLAGS_PYTHON=`$PYTHON $PYCONF CFLAGS 2>&5`
	if test $? -ne 0; then
            AC_MSG_NOTICE([Error configuring Python; building without])
	    rm -f $PYCONF
	    with_python=no
 	else
	    with_python=yes
	fi
fi
		
if test "X$with_python" = "Xyes"; then
	AC_MSG_CHECKING([Name of libpython])
	if expr "$LDFLAGS_PYTHON" : ".*-framework Python" >/dev/null; then
		FRAMEWORKADD="$FRAMEWORKADD -framework Python"
		AC_MSG_RESULT("Using -framework Python")
	else 
		ver=`$PYTHON $PYCONF VERSION`
		if test $? -ne 0; then
			rm -f $PYCONF
			AC_MSG_FAILURE([Python config script failed])
		fi
		# Make sure -lpython is first since it will have lots of symbols to be resolved
		LIBS_PYTHON="-lpython$ver $LIBS_PYTHON"
		AC_MSG_RESULT(should be -lpython$ver)
	fi

	AC_MSG_CHECKING([CFLAGS_PYTHON])
	CFLAGS_PYTHON=`$PYTHON $PYCONF CFLAGS 2>&5`
	if test $? -ne 0; then
	    rm -f $PYCONF
	    if test "X$with_python" = "Xauto"; then
		AC_MSG_FAILURE([Python config script failed])
	    fi
	fi
	AC_MSG_RESULT($CFLAGS_PYTHON)

	AC_MSG_CHECKING([LIBS_PYTHON])
	LIBS_PYTHON="$LIBS_PYTHON `$PYTHON $PYCONF LINKFORSHARED LIBS`"
	if test $? -ne 0; then
		rm -f $PYCONF
		AC_MSG_FAILURE([Python config script failed])
	fi
	AC_MSG_RESULT($LIBS_PYTHON)

	AC_MSG_CHECKING([Location of libpython])
	loc=`$PYTHON $PYCONF LIBPL`
	if test $? -ne 0; then
		rm -f $PYCONF
		AC_MSG_FAILURE([Python config script failed])
	fi
	LDFLAGS_PYTHON="$LDFLAGS_PYTHON -L$loc"
	AC_MSG_RESULT($loc)

	AC_MSG_CHECKING([LIBINST_PYTHON])
	LIBINST_PYTHON=`$PYTHON $PYCONF PYTHON_LIB`
	if test $? -ne 0; then
		rm -f $PYCONF
		AC_MSG_FAILURE([Python config script failed])
	fi
	AC_MSG_RESULT($LIBINST_PYTHON)

	AC_MSG_CHECKING([LDSHARED_PYTHON])
	LDSHARED_PYTHON=`$PYTHON $PYCONF LDSHARED | cut -d\  -f2-`
	if test $? -ne 0; then
		rm -f $PYCONF
		AC_MSG_FAILURE([Python config script failed])
	fi
	AC_MSG_RESULT($LDSHARED_PYTHON)

	EXTRA_STATIC_MODULES="${EXTRA_STATIC_MODULES} libsmacq_python.la"
	OPTIONAL_TESTS="${OPTIONAL_TESTS} python.test.sh pysmacq.test.sh"

	rm -f $PYCONF
fi
AC_SUBST(PYTHON)
AC_SUBST(CFLAGS_PYTHON)
AC_SUBST(LDFLAGS_PYTHON)
AC_SUBST(LIBS_PYTHON)
AC_SUBST(LIBINST_PYTHON)
AC_SUBST(LDSHARED_PYTHON)
AC_SUBST(FRAMEWORKADD) # Apple Frameworks aren't propagated by libtool

AM_CONDITIONAL(USE_PYTHON, test x$with_python = xyes)

##
## gnulib
##
gl_SOURCE_BASE(libgnu)
gl_M4_BASE(libgnu/m4)
AM_CPPFLAGS="${AM_CPPFLAGS} -I\${top_srcdir}/libgnu"
gl_FUNC_GNU_STRFTIME
gl_MODULES(getdate gettime strftime)
gl_GETDATE
gl_INIT


##
## Compiler flags
##
AM_CPPFLAGS="$AM_CPPFLAGS -I\${top_srcdir}/include -D_GNU_SOURCE -D_ISOC99_SOURCE"
if test -n "$GCC"; then
	AM_CPPFLAGS="$AM_CPPFLAGS -Wall -Werror"
fi

##
## Final CFLAGS stuff
##
if test -z "$CO"; then
 	CO="-O9"
fi
AM_CPPFLAGS="$AM_CPPFLAGS $CO"

AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)

##
## Doit
##
AC_CONFIG_FILES([Makefile
                 libgnu/Makefile
                 include/Makefile
                 bin/Makefile
                 pysmacq/Makefile
                 doc/Makefile
                 libsmacq/Makefile
                 misc/Makefile
                 modules/Makefile
                 modules/substr/Makefile
                 modules/patricia/Makefile
                 test/Makefile
                 types/Makefile
		 ])
AC_OUTPUT
