## Process this file with automake to produce Makefile.in.
# Copyright (C) 2004-2006 Free Software Foundation, Inc.
#
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
# Reproduce by: gnulib-tool --import --dir=. --lib=libgnu --source-base=libgnu --m4-base=libgnu/m4 --aux-dir=config --libtool --macro-prefix=gl getdate gettime strftime xalloc-die

AUTOMAKE_OPTIONS = 1.5 gnits no-dependencies

noinst_LTLIBRARIES = libgnu.la

libgnu_la_SOURCES =
libgnu_la_LIBADD = @LTLIBOBJS@
noinst_HEADERS =
EXTRA_DIST =
BUILT_SOURCES =
SUFFIXES =
MOSTLYCLEANFILES =
MOSTLYCLEANDIRS =
CLEANFILES =
DISTCLEANFILES =
MAINTAINERCLEANFILES =

AM_CPPFLAGS =

## begin gnulib module alloca-opt

BUILT_SOURCES += $(ALLOCA_H)
EXTRA_DIST += alloca_.h

# We need the following in order to create <alloca.h> when the system
# doesn't have one that works with the given compiler.
alloca.h: alloca_.h
	cp $(srcdir)/alloca_.h $@-t
	mv $@-t $@
MOSTLYCLEANFILES += alloca.h alloca.h-t

## end   gnulib module alloca-opt

## begin gnulib module allocsa

libgnu_la_SOURCES += allocsa.h allocsa.c
EXTRA_DIST += allocsa.valgrind

## end   gnulib module allocsa

## begin gnulib module exit

libgnu_la_SOURCES += exit.h

## end   gnulib module exit

## begin gnulib module getdate

BUILT_SOURCES += getdate.c
MAINTAINERCLEANFILES += getdate.c
EXTRA_DIST += getdate.c

## end   gnulib module getdate

## begin gnulib module gettext-h

libgnu_la_SOURCES += gettext.h

## end   gnulib module gettext-h

## begin gnulib module setenv

libgnu_la_SOURCES += setenv.h

## end   gnulib module setenv

## begin gnulib module stdbool

BUILT_SOURCES += $(STDBOOL_H)
EXTRA_DIST += stdbool_.h

# We need the following in order to create <stdbool.h> when the system
# doesn't have one that works.
stdbool.h: stdbool_.h
	sed -e 's/@''HAVE__BOOL''@/$(HAVE__BOOL)/g' < $(srcdir)/stdbool_.h > $@-t
	mv $@-t $@
MOSTLYCLEANFILES += stdbool.h stdbool.h-t

## end   gnulib module stdbool

## begin gnulib module xalloc-die

libgnu_la_SOURCES += xalloc-die.c

## end   gnulib module xalloc-die


mostlyclean-local:
	@test -z "$(MOSTLYCLEANDIRS)" || \
	  for dir in $(MOSTLYCLEANDIRS); do \
	    if test -d $$dir; then \
	      echo "rmdir $$dir"; rmdir $$dir; \
	    fi; \
	  done

# Makefile.am ends here
