/* src/include/pg_config.h.  Generated automatically by configure.  */
/*
 * PostgreSQL configuration-settings file.
 *
 * pg_config.h.in is processed by configure to produce pg_config.h.
 *
 * If you want to modify any of the tweakable settings in Part 2
 * of this file, you can do it in pg_config.h.in before running configure,
 * or in pg_config.h afterwards.  Of course, if you edit pg_config.h, then your
 * changes will be overwritten the next time you run configure.
 *
 * $Id: pg_config.h,v 1.1 2002/10/30 19:40:34 mfisk Exp $
 */

#ifndef PG_CONFIG_H
#define PG_CONFIG_H


/*
 *------------------------------------------------------------------------
 * Part 1: feature symbols and limits that are set by configure based on
 * user-supplied switches.  This is first so that stuff in Part 2 can
 * depend on these values.
 *
 * Beware of "fixing" configure-time mistakes by editing these values,
 * since configure may have inserted the settings in other files as well
 * as here.  Best to rerun configure if you forgot --enable-multibyte
 * or whatever.
 *------------------------------------------------------------------------
 */

/* The version number is actually hard-coded into configure.in */
#define PG_VERSION "7.2.1"
/* A canonical string containing the version number, platform, and C compiler */
#define PG_VERSION_STR "PostgreSQL 7.2.1 on i686-pc-linux-gnu, compiled by GCC 2.95.4"

/* Set to 1 if you want LOCALE support (--enable-locale) */
/* #undef USE_LOCALE */

/* Set to 1 if you want cyrillic recode (--enable-recode) */
/* #undef CYR_RECODE */

/* Set to 1 if you want to use multibyte characters (--enable-multibyte) */
/* #undef MULTIBYTE */

#ifdef MULTIBYTE
/* --enable-multibyte implies UNICODE_CONVERSION */
#define UNICODE_CONVERSION
#endif

/* Set to 1 if you want ASSERT checking (--enable-cassert) */
#define USE_ASSERT_CHECKING 1

/* Set to 1 to use syslog() to write postmaster log (--enable-syslog) */
/* #undef ENABLE_SYSLOG */

/* Define to build with Kerberos 4 support (--with-krb4[=DIR]) */
/* #undef KRB4 */

/* Define to build with Kerberos 5 support (--with-krb5[=DIR]) */
/* #undef KRB5 */

/* Kerberos name of the Postgres service principal (--with-krb-srvnam=NAME) */
#define PG_KRB_SRVNAM "postgres"

/* Define to build with (Open)SSL support (--with-openssl[=DIR]) */
/* #undef USE_SSL */

/* Define to build with PAM Support */
/* #undef USE_PAM */

/* 
 * DEF_PGPORT is the TCP port number on which the Postmaster listens and
 * which clients will try to connect to.  This is just a default value;
 * it can be overridden at postmaster or client startup.  It's awfully
 * convenient if your clients have the right default compiled in, though.
 * (--with-pgport=PORTNUM)
 */ 
#define DEF_PGPORT 5432
/* ... and once more as a string constant instead */
#define DEF_PGPORT_STR "5432"

/*
 * Default soft limit on number of backend server processes per postmaster;
 * this is just the default setting for the postmaster's -N switch.
 * (--with-maxbackends=N)
 */
#define DEF_MAXBACKENDS 32

/* --enable-nls */
/* #undef ENABLE_NLS */

/* location of locale files */
/* #undef LOCALEDIR */

/* Define to build the ODBC driver for unixODBC */
/* #undef WITH_UNIXODBC */

/* Define to build the ODBC driver for iODBC */
/* #undef WITH_IODBC */


/*
 *------------------------------------------------------------------------
 * Part 2: feature symbols and limits that are user-configurable, but
 * only by editing this file ... there's no configure support for them.
 *
 * Editing this file and doing a full rebuild (and an initdb if noted)
 * should be sufficient to change any of these.
 *------------------------------------------------------------------------
 */

/*
 * Default number of buffers in shared buffer pool (each of size BLCKSZ).
 * This is just the default setting for the postmaster's -B switch.
 * Perhaps it ought to be configurable from a configure switch.
 * NOTE: default setting corresponds to the minimum number of buffers
 * that postmaster.c will allow for the default MaxBackends value.
 */
#define DEF_NBUFFERS (DEF_MAXBACKENDS > 8 ? DEF_MAXBACKENDS * 2 : 16)

/*
 * Size of a disk block --- this also limits the size of a tuple.
 * You can set it bigger if you need bigger tuples (although TOAST
 * should reduce the need to have large tuples, since fields can now
 * be spread across multiple tuples).
 *
 * The maximum possible value of BLCKSZ is currently 2^15 (32768).
 * This is determined by the 15-bit widths of the lp_off and lp_len
 * fields in ItemIdData (see include/storage/itemid.h).
 *
 * CAUTION: changing BLCKSZ requires an initdb.
 */
#define BLCKSZ	8192

/*
 * RELSEG_SIZE is the maximum number of blocks allowed in one disk file.
 * Thus, the maximum size of a single file is RELSEG_SIZE * BLCKSZ;
 * relations bigger than that are divided into multiple files.
 *
 * CAUTION: RELSEG_SIZE * BLCKSZ must be less than your OS' limit on file
 * size.  This is typically 2Gb or 4Gb in a 32-bit operating system.  By
 * default, we make the limit 1Gb to avoid any possible integer-overflow
 * problems within the OS.  A limit smaller than necessary only means we
 * divide a large relation into more chunks than necessary, so it seems
 * best to err in the direction of a small limit.  (Besides, a power-of-2
 * value saves a few cycles in md.c.)
 *
 * CAUTION: changing RELSEG_SIZE requires an initdb.
 */
#define RELSEG_SIZE	(0x40000000 / BLCKSZ)

/*
 * Maximum number of columns in an index and maximum number of arguments
 * to a function. They must be the same value.
 *
 * The minimum value is 8 (index creation uses 8-argument functions).
 * There is no specific upper limit, although large values will waste
 * system-table space and processing time.
 *
 * CAUTION: changing these requires an initdb.
 *
 * BTW: if you need to call dynamically-loaded old-style C functions that
 * have more than 16 arguments, you will also need to add cases to the
 * switch statement in fmgr_oldstyle() in src/backend/utils/fmgr/fmgr.c.
 * But consider converting such functions to new-style instead...
 */
#define INDEX_MAX_KEYS		16
#define FUNC_MAX_ARGS		INDEX_MAX_KEYS

/*
 * System default value for pg_attribute.attstattarget
 */
#define DEFAULT_ATTSTATTARGET	10

/*
 * Define this to make libpgtcl's "pg_result -assign" command process C-style
 * backslash sequences in returned tuple data and convert Postgres array
 * attributes into Tcl lists.  CAUTION: this conversion is *wrong* unless
 * you install the routines in contrib/string/string_io to make the backend
 * produce C-style backslash sequences in the first place.
 */
/* #define TCL_ARRAYS */

/*
 * User locks are handled totally on the application side as long term
 * cooperative locks which extend beyond the normal transaction boundaries.
 * Their purpose is to indicate to an application that someone is `working'
 * on an item.  Define this flag to enable user locks.  You will need the
 * loadable module user-locks.c to use this feature.
 */
#define USER_LOCKS

/*
 * Define this if you want psql to _always_ ask for a username and a password
 * for password authentication.
 */
/* #define PSQL_ALWAYS_GET_PASSWORDS */

/*
 * Define this if you want to allow the lo_import and lo_export SQL functions
 * to be executed by ordinary users.  By default these functions are only
 * available to the Postgres superuser.  CAUTION: these functions are
 * SECURITY HOLES since they can read and write any file that the Postgres
 * backend has permission to access.  If you turn this on, don't say we
 * didn't warn you.
 */
/* #define ALLOW_DANGEROUS_LO_FUNCTIONS */

/*
 * Use btree bulkload code: 
 * this code is moderately slow (~10% slower) compared to the regular
 * btree (insertion) build code on sorted or well-clustered data.  on
 * random data, however, the insertion build code is unusable -- the
 * difference on a 60MB heap is a factor of 15 because the random
 * probes into the btree thrash the buffer pool.
 *
 * Great thanks to Paul M. Aoki (aoki@CS.Berkeley.EDU)
 */
#define FASTBUILD /* access/nbtree/nbtsort.c */

/*
 * MAXPGPATH: standard size of a pathname buffer in Postgres (hence,
 * maximum usable pathname length is one less).
 *
 * We'd use a standard system header symbol for this, if there weren't
 * so many to choose from: MAXPATHLEN, _POSIX_PATH_MAX, MAX_PATH, PATH_MAX
 * are all defined by different "standards", and often have different
 * values on the same platform!  So we just punt and use a reasonably
 * generous setting here.
 */
#define MAXPGPATH		1024

/*
 * DEFAULT_MAX_EXPR_DEPTH: default value of max_expr_depth SET variable.
 */
#define DEFAULT_MAX_EXPR_DEPTH	10000

/*
 * PG_SOMAXCONN: maximum accept-queue length limit passed to listen(2).
 * You'd think we should use SOMAXCONN from <sys/socket.h>, but on many
 * systems that symbol is much smaller than the kernel's actual limit.
 * In any case, this symbol need be twiddled only if you have a kernel
 * that refuses large limit values, rather than silently reducing the
 * value to what it can handle (which is what most if not all Unixen do).
 */
#define PG_SOMAXCONN	10000

/*
 * You can try changing this if you have a machine with bytes of another
 * size, but no guarantee...
 */
#define BITS_PER_BYTE		8

/*
 * Define this if your operating system supports AF_UNIX family sockets.
 */
#if !defined(__QNX__) && !defined(__BEOS__)
# define HAVE_UNIX_SOCKETS 1
#endif

/*
 * This is the default directory in which AF_UNIX socket files are placed.
 * Caution: changing this risks breaking your existing client applications,
 * which are likely to continue to look in the old directory.  But if you
 * just hate the idea of sockets in /tmp, here's where to twiddle it.
 * You can also override this at runtime with the postmaster's -k switch.
 */
#define DEFAULT_PGSOCKET_DIR  "/tmp"


/*
 *------------------------------------------------------------------------
 * These hand-configurable symbols are for enabling debugging code,
 * not for controlling user-visible features or resource limits.
 *------------------------------------------------------------------------
 */

/* Define this to cause pfree()'d memory to be cleared immediately,
 * to facilitate catching bugs that refer to already-freed values.
 * XXX For 7.1 development, define this automatically if --enable-cassert.
 * In the long term it probably doesn't need to be on by default.
 */
#ifdef USE_ASSERT_CHECKING
#define CLOBBER_FREED_MEMORY
#endif

/* Define this to check memory allocation errors (scribbling on more
 * bytes than were allocated).
 * XXX For 7.1 development, define this automatically if --enable-cassert.
 * In the long term it probably doesn't need to be on by default.
 */
#ifdef USE_ASSERT_CHECKING 
#define MEMORY_CONTEXT_CHECKING
#endif

/* Define this to force all parse and plan trees to be passed through
 * copyObject(), to facilitate catching errors and omissions in copyObject().
 */
/* #define COPY_PARSE_PLAN_TREES */

/* Enable debugging print statements in the date/time support routines. */
/* #define DATEDEBUG */

/* Enable debugging print statements for lock-related operations. */
/* #define LOCK_DEBUG */

/*
 * Other debug #defines (documentation, anyone?)
 */
/* #define IPORTAL_DEBUG  */
/* #define HEAPDEBUGALL  */
/* #define ISTRATDEBUG  */
/* #define ACLDEBUG */
/* #define RTDEBUG */
/* #define GISTDEBUG */

/*
 * defining unsafe floats will make float4 and float8 ops faster
 * by suppressing overflow/underflow checks.
 */
/* #define UNSAFE_FLOATS */


/*
 *------------------------------------------------------------------------
 * Part 3: system configuration information that is auto-detected by
 * configure.  In theory you shouldn't have to touch any of this stuff
 * by hand.  In the real world, configure might get it wrong...
 *------------------------------------------------------------------------
 */

/* Define const as empty if your compiler doesn't grok const. */
/* #undef const */

/* Define as your compiler's spelling of "inline", or empty if no inline. */
/* #undef inline */

/* Define as empty if the C compiler doesn't understand "signed". */
/* #undef signed */

/* Define as empty if the C compiler doesn't understand "volatile". */
/* #undef volatile */

/* Define if your cpp understands the ANSI stringizing operators in macros */
#define HAVE_STRINGIZE 1

/* Set to 1 if you have <crypt.h> */
#define HAVE_CRYPT_H 1

/* Set to 1 if you have <dld.h> */
/* #undef HAVE_DLD_H */

/* Set to 1 if you have <endian.h> */
#define HAVE_ENDIAN_H 1

/* Set to 1 if you have <fp_class.h> */
/* #undef HAVE_FP_CLASS_H */

/* Set to 1 if you have <getopt.h> */
#define HAVE_GETOPT_H 1

/* Set to 1 if you have <history.h> */
/* #undef HAVE_HISTORY_H */

/* Set to 1 if you have <ieeefp.h> */
/* #undef HAVE_IEEEFP_H */

/* Set to 1 if you have <netinet/tcp.h> */
#define HAVE_NETINET_TCP_H 1

/* Set to 1 if you have <readline.h> */
/* #undef HAVE_READLINE_H */

/* Set to 1 if you have <readline/history.h> */
/* #undef HAVE_READLINE_HISTORY_H */

/* Set to 1 if you have <readline/readline.h> */
/* #undef HAVE_READLINE_READLINE_H */

/* Set to 1 if you have <sys/ipc.h> */
#define HAVE_SYS_IPC_H 1

/* Set to 1 if  you have <sys/select.h> */
#define HAVE_SYS_SELECT_H 1

/* Set to 1 if you have <sys/un.h> */
#define HAVE_SYS_UN_H 1

/* Set to 1 if you have <sys/sem.h> */
#define HAVE_SYS_SEM_H 1

/* Set to 1 if you have <sys/shm.h> */
#define HAVE_SYS_SHM_H 1

/* Set to 1 if you have <kernel/OS.h> */
/* #undef HAVE_KERNEL_OS_H */

/* Set to 1 if you have <SupportDefs.h> */
/* #undef HAVE_SUPPORTDEFS_H */

/* Set to 1 if you have <kernel/image.h> */
/* #undef HAVE_KERNEL_IMAGE_H */

/* Set to 1 if you have <termios.h> */
#define HAVE_TERMIOS_H 1

/* Set to 1 if you have <sys/pstat.h> */
/* #undef HAVE_SYS_PSTAT_H */

/* Define if string.h and strings.h may both be included */
#define STRING_H_WITH_STRINGS_H 1

/* Define if you have the setproctitle function.  */
/* #undef HAVE_SETPROCTITLE */

/* Define if you have the pstat function. */
/* #undef HAVE_PSTAT */

/* Define if the PS_STRINGS thing exists. */
/* #undef HAVE_PS_STRINGS */

/* Define if you have the stricmp function.  */
/* #undef HAVE_STRICMP */

/* Set to 1 if you have history functions (either in libhistory or libreadline) */
/* #undef HAVE_HISTORY_FUNCTIONS */

/* Set to 1 if you have <pwd.h> */
#define HAVE_PWD_H 1

/* Set to 1 if you have gettimeofday(a) instead of gettimeofday(a,b) */
/* #undef GETTIMEOFDAY_1ARG */
#ifdef GETTIMEOFDAY_1ARG
# define gettimeofday(a,b) gettimeofday(a)
#endif

/* Set to 1 if you have snprintf() in the C library */
#define HAVE_SNPRINTF 1

/* Set to 1 if your standard system headers declare snprintf() */
#define HAVE_SNPRINTF_DECL 1

/* Set to 1 if you have vsnprintf() in the C library */
#define HAVE_VSNPRINTF 1

/* Set to 1 if your standard system headers declare vsnprintf() */
#define HAVE_VSNPRINTF_DECL 1

/* Set to 1 if you have strerror() */
#define HAVE_STRERROR 1

/* Set to 1 if you have isinf() */
#define HAVE_ISINF 1
#ifndef HAVE_ISINF
extern int isinf(double x);
#endif

/*
 *	These are all related to port/isinf.c 
 */
/* #undef HAVE_FPCLASS */
/* #undef HAVE_FP_CLASS */
/* #undef HAVE_FP_CLASS_H */
/* #undef HAVE_FP_CLASS_D */
/* #undef HAVE_CLASS */

/* Set to 1 if you have gethostname() */
#define HAVE_GETHOSTNAME 1
#ifndef HAVE_GETHOSTNAME
extern int gethostname(char *name, int namelen);
#endif

/* Set to 1 if struct tm has a tm_zone member */
#define HAVE_TM_ZONE 1

/* Set to 1 if you have int timezone.
 * NOTE: if both tm_zone and a global timezone variable exist,
 * using the tm_zone field should probably be preferred,
 * since global variables are inherently not thread-safe.
 */
#define HAVE_INT_TIMEZONE 

/* Set to 1 if you have cbrt() */
#define HAVE_CBRT 1

/* Set to 1 if you have inet_aton() */
#define HAVE_INET_ATON 1

#ifndef HAVE_INET_ATON
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
extern int inet_aton(const char *cp, struct in_addr * addr);
#endif

/* Set to 1 if you have fcvt() */
#define HAVE_FCVT 1

/* Set to 1 if you have rint() */
#define HAVE_RINT 1 

/* Set to 1 if you have finite() */
#define HAVE_FINITE 1

/* Set to 1 if you have memmove() */
#define HAVE_MEMMOVE 1

/* Set to 1 if you have sigsetjmp() */
#define HAVE_SIGSETJMP 1

/*
 * When there is no sigsetjmp, its functionality is provided by plain
 * setjmp. Incidentally, nothing provides setjmp's functionality in
 * that case.
 */
#ifndef HAVE_SIGSETJMP
# define sigjmp_buf jmp_buf
# define sigsetjmp(x,y)	setjmp(x)
# define siglongjmp longjmp
#endif

/* Set to 1 if you have sysconf() */
#define HAVE_SYSCONF 1

/* Set to 1 if you have getrusage() */
#define HAVE_GETRUSAGE 1

/* Set to 1 if you have waitpid() */
#define HAVE_WAITPID 1

/* Set to 1 if you have setsid() */
#define HAVE_SETSID 1

/* Set to 1 if you have sigprocmask() */
#define HAVE_SIGPROCMASK 1

/* Set to 1 if you have sigprocmask() */
#define HAVE_STRCASECMP 1
#ifndef HAVE_STRCASECMP
extern int strcasecmp(char *s1, char *s2);
#endif

/* Set to 1 if you have strtol() */
#define HAVE_STRTOL 1

/* Set to 1 if you have strtoul() */
#define HAVE_STRTOUL 1

/* Set to 1 if you have strdup() */
#define HAVE_STRDUP 1
#ifndef HAVE_STRDUP
extern char *strdup(char const *);
#endif

/* Set to 1 if you have random() */
#define HAVE_RANDOM 1
#ifndef HAVE_RANDOM
extern long random(void);
#endif

/* Set to 1 if you have srandom() */
#define HAVE_SRANDOM 1
#ifndef HAVE_SRANDOM
extern void srandom(unsigned int seed);
#endif

/* The random() function is expected to yield values 0 .. MAX_RANDOM_VALUE */
/* Currently, all known implementations yield 0..2^31-1, so we just hardwire
 * this constant.  We could do a configure test if it proves to be necessary.
 * CAUTION: Think not to replace this with RAND_MAX.  RAND_MAX defines the
 * maximum value of the older rand() function, which is often different from
 * --- and considerably inferior to --- random().
 */
#define MAX_RANDOM_VALUE  (0x7FFFFFFF)

/* Define if you have dlopen() */
#define HAVE_DLOPEN 1

/* Define if you have fdatasync() */
#define HAVE_FDATASYNC 1

/* Define if the standard header unistd.h declares fdatasync() */
#define HAVE_FDATASYNC_DECL 1

#if defined(HAVE_FDATASYNC) && !defined(HAVE_FDATASYNC_DECL)
extern int fdatasync(int fildes);
#endif

/* Set to 1 if you have libz.a */
#define HAVE_LIBZ 1

/* Set to 1 if you have libreadline.a */
/* #undef HAVE_LIBREADLINE */

/* Set to 1 if you have libhistory.a */
/* #undef HAVE_LIBHISTORY */

/* Set to 1 if your libreadline defines rl_completion_append_character */
/* #undef HAVE_RL_COMPLETION_APPEND_CHARACTER */

/* Set to 1 if you have rl_completion_matches */
/* #undef HAVE_RL_COMPLETION_MATCHES */

/* Set to 1 if you have rl_filename_completion_function */
/* #undef HAVE_RL_FILENAME_COMPLETION_FUNCTION */

/* Set to 1 if you have getopt_long() (GNU long options) */
#define HAVE_GETOPT_LONG 1

/* Set to 1 if you have union semun */
/* #undef HAVE_UNION_SEMUN */

/* Set to 1 if you have struct cmsgcred */
/* #undef HAVE_STRUCT_CMSGCRED */

/* Set to 1 if you have struct fcred */
/* #undef HAVE_STRUCT_FCRED */

/* Set to 1 if you have struct sockcred */
/* #undef HAVE_STRUCT_SOCKCRED */

/* Set to 1 if you have struct sockaddr_un */
#define HAVE_STRUCT_SOCKADDR_UN 1

/* Set to 1 if type "long int" works and is 64 bits */
/* #undef HAVE_LONG_INT_64 */

/* Set to 1 if type "long long int" works and is 64 bits */
#define HAVE_LONG_LONG_INT_64 

/* Set to 1 if type "long long int" constants should be suffixed by LL */
#define HAVE_LL_CONSTANTS 1

/* Define this as the appropriate snprintf format for 64-bit ints, if any */
#define INT64_FORMAT "%lld"

/*
 * We need a #define symbol for sizeof(Datum) for use in some #if tests.
 */
#define SIZEOF_DATUM 4

/*
 * These must be defined as the alignment requirement (NOT the size) of
 * each of the basic C data types (except char, which we assume has align 1).
 * MAXIMUM_ALIGNOF is the largest alignment requirement for any C data type.
 * ALIGNOF_LONG_LONG_INT need only be defined if HAVE_LONG_LONG_INT_64 is.
 */
#define ALIGNOF_SHORT 2
#define ALIGNOF_INT 4
#define ALIGNOF_LONG 4
#define ALIGNOF_LONG_LONG_INT 4
#define ALIGNOF_DOUBLE 4
#define MAXIMUM_ALIGNOF 4

/* Define as the type of the 3rd argument to accept() */
#define ACCEPT_TYPE_ARG3 size_t

/* Define if POSIX signal interface is available */
#define HAVE_POSIX_SIGNALS 

/* Define if C++ compiler accepts "using namespace std" */
/* #undef HAVE_NAMESPACE_STD */

/* Define if C++ compiler accepts "#include <string>" */
/* #undef HAVE_CXX_STRING_HEADER */

/* Define if you have the optreset variable */
/* #undef HAVE_INT_OPTRESET */

/* Define if you have strtoll() */
#define HAVE_STRTOLL 1

/* Define if you have strtoq() */
/* #undef HAVE_STRTOQ */

/* If strtoq() exists, rename it to the more standard strtoll() */
#if defined(HAVE_LONG_LONG_INT_64) && !defined(HAVE_STRTOLL) && defined(HAVE_STRTOQ)
# define strtoll strtoq
# define HAVE_STRTOLL 1
#endif

/* Define if you have strtoull() */
#define HAVE_STRTOULL 1

/* Define if you have strtouq() */
/* #undef HAVE_STRTOUQ */

/* If strtouq() exists, rename it to the more standard strtoull() */
#if defined(HAVE_LONG_LONG_INT_64) && !defined(HAVE_STRTOULL) && defined(HAVE_STRTOUQ)
# define strtoull strtouq
# define HAVE_STRTOULL 1
#endif

/* Define if you have atexit() */
#define HAVE_ATEXIT 1

/* Define if you have on_exit() */
/* #undef HAVE_ON_EXIT */

/* Define if the corresponding types are defined in standard headers */
/* #undef HAVE_INT8 */
/* #undef HAVE_UINT8 */
/* #undef HAVE_INT64 */
/* #undef HAVE_UINT64 */
#define HAVE_SIG_ATOMIC_T 1


/*
 *------------------------------------------------------------------------
 * Part 4: pull in system-specific declarations.
 *
 * This is still configure's responsibility, because it picks where
 * the "pg_config_os.h" symlink points...
 *------------------------------------------------------------------------
 */

/*
 * Pull in OS-specific declarations (using link created by configure)
 */

#include "pg_config_os.h"

/*
 * Provide default definitions for things that port-specific pg_config_os.h
 * files can set, but typically don't.
 */

#ifndef DLLIMPORT
#define DLLIMPORT				/* no special DLL markers on most ports */
#endif

/*
 * The following is used as the arg list for signal handlers.  Any ports
 * that take something other than an int argument should override this in
 * their pg_config_os.h file.  Note that variable names are required
 * because it is used in both the prototypes as well as the definitions.
 * Note also the long name.  We expect that this won't collide with
 * other names causing compiler warnings.
 */ 

#ifndef SIGNAL_ARGS
#define SIGNAL_ARGS  int postgres_signal_arg
#endif


#endif /* PG_CONFIG_H */
