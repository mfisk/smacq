/* dump.h
 *
 * Copyright (C) 2004 Brad Robel-Forrest
 *
 * Released to the public domain.
 */
#ifndef DUMP_H_
#define DUMP_H_


#include <unistd.h>


#if !defined (DUMP_ENABLE)
  #define DUMP_USE_NOTHING
#endif


/*
 * Choose how/where to write output by simply adding one of these defines
 * before you include this header file...
 *
 * #define DUMP_USE_SYSLOG
 * #define DUMP_USE_PRINTK
 * #define DUMP_USE_FILELOG
 * #define DUMP_USE_NOTHING
 *
 * ...if none are defined, then we default to printf against stdout.
 *
 * The DUMP_USE_SYSLOG define will write all your output to your syslog
 * handler.
 *
 * The DUMP_USE_PRINTK define will write all your output to the kernel
 * printer (on by default when compiling for the kernel).
 *
 * The DUMP_USE_FILELOG define will write all your output into a file.
 *
 * Note that you can leave all your tags in and define DUMP_USE_NOTHING
 * which will compile them out so there will be no efficiency impact
 * under normal conditions.
 */
#if defined (DUMP_USE_NOTHING)
  #define DUMP_NOTHING( fmt, args... )

  #define DUMP_FUNC	DUMP_NOTHING
  #define DUMP_LEVEL
  #define DUMP_NL
  #define DUMP_PID

  #ifndef DUMP_NO_DUMP_BUF
    #define DUMP_NO_DUMP_BUF
  #endif

  #define dump_buf( h, b, s )

  #ifdef DUMP_USE_FILELOG
    #undef DUMP_USE_FILELOG
  #endif

#elif defined (DUMP_USE_SYSLOG)
  #include <syslog.h>
  #include <stdio.h>
  #include <ctype.h>

  #define DUMP_FUNC	syslog
  #define DUMP_LEVEL	LOG_DEBUG,
  #define DUMP_NL	""
  #define DUMP_PID	getpid()

#elif defined (DUMP_USE_PRINTK) || defined (__KERNEL__)
  #include <linux/kernel.h>

  #define DUMP_FUNC	printk
  #define DUMP_LEVEL	KERN_DEBUG
  #define DUMP_NL	"\n"
  #define DUMP_PID	-1
  #define isgraph(c)	( (c) > 32 && (c) < 127 )

#elif defined (DUMP_USE_FILELOG)
  #include <stdio.h>
  #include <string.h>
  #include <stdarg.h>
  #include <ctype.h>

  #define DUMP_FUNC	dump_filelog
  #define DUMP_LEVEL
  #define DUMP_NL	"\n"
  #define DUMP_PID	getpid()

  #ifndef DUMP_FILELOG_FILENAME
    #define DUMP_FILELOG_FILENAME	"output.dump"
  #endif

#else
  #include <stdio.h>
  #include <ctype.h>

  #define DUMP_FUNC	fprintf
  #define DUMP_LEVEL	stdout,
  #define DUMP_NL	"\n"
  #define DUMP_PID	getpid()
#endif

#ifndef DUMP_PRE_FMT
  #define DUMP_PRE_FMT    "%s:%d %s:%d "
#endif
#ifndef DUMP_PRE_ARGS
  #define DUMP_PRE_ARGS   __FILE__, (int)DUMP_PID, __FUNCTION__, __LINE__
#endif


/*
 * Set a breakpoint to cause a trap signal to occur.
 */
#if defined (__i386__) && defined (__GNUC__) && __GNUC__ >= 2
#define	DUMP_BREAKPOINT()    do{ __asm__ __volatile__ ("int $03"); } while( 0 )
#elif defined (__alpha__) && defined (__GNUC__) && __GNUC__ >= 2
#define	DUMP_BREAKPOINT()    do{ __asm__ __volatile__ ("bpt"); } while( 0 )
#elif defined (__ppc__)
#define DUMP_BREAKPOINT()    do{ __asm__ __volatile__ ("trap"); } while( 0 )
#else	/* !__i386__ && !__alpha__ && !__ppc__ */
#define	DUMP_BREAKPOINT()
#endif	/* __i386__ */


/*
 * Set a breakpoint on a particular condition.
 */
#define DUMP_BREAKCOND( e )  if( (e) ) DUMP_BREAKPOINT();


/* `printf' debugging! */
#define DUMP()		DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT DUMP_NL, DUMP_PRE_ARGS )

/*
 * Note that you can give these guys pretty much anything and it should
 * just do what you'd expect...
 *
 * foo->bar = "blah";
 * DUMP_s( foo->bar );
 *
 * ...would print "foo->bar = blah" and...
 *
 * u_long blah = 0x12345678;
 * DUMP_lx( ntohl( blah ));
 *
 * ...would print "ntohl( blah ) = 0x78563412".
 */

/* `int' arguments */
#define DUMP_d( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %d" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_i( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %i" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_o( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#o" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_u( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %u" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_x( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0x%08x" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_X( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0X%08X" DUMP_NL, DUMP_PRE_ARGS, #v, v )

/* `short' arguments */
#define DUMP_hd( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %hd" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_hi( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %hi" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_ho( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#ho" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_hu( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %hu" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_hx( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0x%04hx" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_hX( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0X%04hX" DUMP_NL, DUMP_PRE_ARGS, #v, v )

/* `long' arguments */
#define DUMP_ld( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %ld" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_li( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %li" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_lo( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#lo" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_lu( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %lu" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_lx( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0x%08lx" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_lX( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0X%08lX" DUMP_NL, DUMP_PRE_ARGS, #v, v )

/* `size_t' arguments */
#define DUMP_Zd( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %Zd" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_Zi( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %Zi" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_Zo( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#Zo" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_Zu( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %Zu" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_Zx( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0x%08Zx" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_ZX( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = 0X%08ZX" DUMP_NL, DUMP_PRE_ARGS, #v, v )

/* `double' arguments (regular and long) */
#define DUMP_e( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#e" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_Le( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#Le" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_E( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#E" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_LE( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#LE" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_f( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#f" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_Lf( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#Lf" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_g( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#g" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_Lg( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#Lg" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_G( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#G" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_LG( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %#LG" DUMP_NL, DUMP_PRE_ARGS, #v, v )

/* string, character, and pointer arguments */
#define DUMP_s( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %s" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_c( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %c" DUMP_NL, DUMP_PRE_ARGS, #v, v )
#define DUMP_p( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %p" DUMP_NL, DUMP_PRE_ARGS, #v, v )

/* IP address */
#define DUMP_IP( v )	DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT "%s = %d.%d.%d.%d" DUMP_NL, DUMP_PRE_ARGS, #v, \
				   ( int )((( v ) >> 0  ) & 0xff ), \
				   ( int )((( v ) >> 8  ) & 0xff ), \
				   ( int )((( v ) >> 16 ) & 0xff ), \
				   ( int )((( v ) >> 24 ) & 0xff ))

/* text (a variable arg macro that is printf-like) */
#define DUMP_msg( f, args... ) DUMP_FUNC( DUMP_LEVEL DUMP_PRE_FMT f DUMP_NL, \
                                          DUMP_PRE_ARGS , ##args )


/*
 * You can define this macro if you aren't going to use dump_buf and are sick
 * of seeing the compiler warn you about it.
 *
 * Or, if you want it available, but may not always use it, add a call to it
 * right after a return...
 * 
 * int func( void ) {
 *   blah;
 *   blah;
 *   blah;
 *   return 0;
 *   dump_buf( NULL, NULL, 0 );
 * }
 *
 * ...so that the compiler won't bitch but you still have the func defined
 * for use later.
 */
#ifndef DUMP_NO_DUMP_BUF

/*
 * Use this macro to simplify the call so that it uses the stringified
 * value of the buffer's name for the header.
 */
#define DUMP_BUF( b, s )	dump_buf( #b, b, s )

#define dump_minimum(a,b)	(((a) < (b)) ? (a) : (b))

/*
 * This will dump out a buffer in a hex, decimal, short, long, and ascii
 * table.
 */
static __inline void dump_buf(
    const char		*header,
    void		*ubuf,
    size_t		bufSize
)
{
    unsigned char	*buf = ( unsigned char * )ubuf;
    char		b[128];
    size_t		n = 0;
    size_t		m;
    size_t		o;


    if( header ) {
	DUMP_FUNC(DUMP_LEVEL "%s (%lu bytes):" DUMP_NL, header,
			(unsigned long)bufSize);
    }

    if (buf == NULL) {
        DUMP_FUNC(DUMP_LEVEL "    (NULL)" DUMP_NL);
	return;
    }

    if (bufSize == 0) {
        DUMP_FUNC(DUMP_LEVEL "    (EMPTY)" DUMP_NL);
	return;
    }

    for (m = 0; m < bufSize; m += 4) { 
	n += sprintf(b+n, "    ");

        /*
         * Write out four columns of hexadecimal numbers.
         */
	for (o = m; o < m + 4; o++) {
            if( o < bufSize ) {
                n += sprintf(b+n,  "%02x ", (unsigned char)buf[o]);
            } else {
                n += sprintf(b+n,  "   " );
            }
	}

	n += sprintf(b+n,  "| ");

        /*
         * Write out four columns of decimal numbers.
         */
	for (o = m; o < m + 4; o++) {
            if( o < bufSize ) {
                n += sprintf(b+n,  "%3u ", (unsigned int)buf[o]);
            } else {
                n += sprintf(b+n,  "    " );
            }
	}

	n += sprintf(b+n,  "| ");

        /*
         * Write out two columns of converted unsigned integers.
         */
        if( m + 2 <= bufSize ) {
            n += sprintf(b+n,  "%6u ", 
                         (unsigned int)
                         (((unsigned char)buf[m + 0] << 8) | ((unsigned char)buf[m + 1])));
        } else if( m + 1 <= bufSize ) {
            n += sprintf(b+n,  "%6u ", 
                         (unsigned int)
                         (((unsigned char)buf[m + 0] << 8) | 0) );
        } else {
            n += sprintf(b+n,  "%6s ", "(end)");
        }

        if( m + 4 <= bufSize ) {
            n += sprintf(b+n,  "%6u ", 
                         (unsigned int)
                         (((unsigned char)buf[m + 2] << 8) | ((unsigned char)buf[m + 3])));
        } else if( m + 3 <= bufSize ) {
            n += sprintf(b+n,  "%6u ", 
                         (unsigned int)
                         (((unsigned char)buf[m + 2] << 8) | 0) );
        } else {
            n += sprintf(b+n,  "%6s ", "(end)");
        }

	n += sprintf(b+n,  "| ");

        /*
         * Write out one column of converted unsigned longs.
         */
	if (m + 4 <= bufSize) {
	    n += sprintf(b+n,  "%10lu ", 
		(unsigned long)
		( ((unsigned char)buf[m + 0] << 24)
		| ((unsigned char)buf[m + 1] << 16)
		| ((unsigned char)buf[m + 2] << 8)
		| ((unsigned char)buf[m + 3]) ));
        } else if( m + 3 <= bufSize ) {
	    n += sprintf(b+n,  "%10lu ", 
		(unsigned long)
		( ((unsigned char)buf[m + 0] << 24)
		| ((unsigned char)buf[m + 1] << 16)
		| ((unsigned char)buf[m + 2] << 8)
		| 0) );
        } else if( m + 2 <= bufSize ) {
	    n += sprintf(b+n,  "%10lu ", 
		(unsigned long)
		( ((unsigned char)buf[m + 0] << 24)
		| ((unsigned char)buf[m + 1] << 16)
		| 0) );
        } else if( m + 1 <= bufSize ) {
	    n += sprintf(b+n,  "%10lu ", 
		(unsigned long)
		( ((unsigned char)buf[m + 0] << 24)
		| 0) );
	} else {
	    n += sprintf(b+n,  "%10s ", "(end)");
	}
	
	n += sprintf(b+n, "| ");

	for (o = m; o < dump_minimum(bufSize, m + 4); o++) {
	    n += sprintf(b+n, "%c", isgraph(buf[o]) ? buf[o] : '.');
	}
	
	DUMP_FUNC(DUMP_LEVEL "%s" DUMP_NL, b);
	n = 0;
    }
}

#else

#define DUMP_BUF( b, s )

#endif /* DUMP_NO_DUMP_BUF */


#ifdef DUMP_USE_FILELOG

/*
 * Calling the open and close is optional. If you don't, it will
 * default to open a file called 'output.dump' in the current
 * directory (or whatever the user defines in DUMP_FILELOG_FILENAME)
 * and it will never close it.
 *
 * You can call dump_filelog_open at anytime before or after you call
 * dump_filelog. By calling it your are either changing the filename
 * to write the output into or truncating the current one (if you
 * open the same filename). You don't have to call dump_filelog_close
 * before you call dump_filelog_open (it'll close it before it opens
 * a new one).
 */
#define dump_filelog_open( fn )		_dump_filelog( fn, NULL )
#define dump_filelog( fmt, args... )	_dump_filelog( NULL, fmt, ##args )
#define dump_filelog_close()		_dump_filelog( NULL, NULL )

static __inline void _dump_filelog(
    const char *	fn,
    const char * const	fmt,
    ...
)
{
    static FILE		*fp = NULL;
    va_list		args;


    if( fn == NULL && fmt != NULL && fp == NULL ) {
	/*
	 * Here we were called with the expectation that we have an
	 * open file, yet we don't have one set up. In this case,
	 * we'll just open one up with a default name.
	 */
	fn = DUMP_FILELOG_FILENAME;
    }
    
    if( fn != NULL && fn[0] != '\0' ) {
	if( fp != NULL ) {
	    fclose( fp );
	}
	/*
	 * We were given a filename. This tells us to truncate it...even if it's
	 * the same name.
	 */
	fp = fopen( fn, "w" );
    }

    if( fn == NULL && fmt == NULL && fp != NULL ) {
	/*
	 * When both args are empty, we will close everything down.
	 */
	fclose( fp );
	fp = NULL;
    }

    if( fmt != NULL && fp != NULL ) {
	/*
	 * Woohoo! We get to write out something. If the string doesn't end in a
	 * newline, we will add one.
	 */
	va_start( args, fmt );
	vfprintf( fp, fmt, args );
	va_end( args );
	if( fmt[ strlen( fmt ) - 1 ] != '\n' ) {
	    fprintf( fp, "\n" );
	}
    }
}

#endif /* DUMP_USE_FILELOG */


#endif /* DUMP_H_ */
