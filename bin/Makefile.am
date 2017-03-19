bin_PROGRAMS = smacqq #smacqq-static #iterative-example
##SHELL=PATH=$(top_srcdir)/misc:$$PATH bash

if GASNET
smacqq_SOURCES = psmacqq.cpp 
smacqq_CPPFLAGS = $(AM_CPPFLAGS) -fpermissive -Wno-error -Wno-unused # GASnet code isn't very pedantic
else
smacqq_SOURCES = smacqq.cpp 
smacqq_CPPFLAGS = $(AM_CPPFLAGS)
#smacqq_static_SOURCES = smacqq.cpp 
endif

smacqq_DEPENDENCIES = $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libsmacq/libsmacq.la
smacqq_LDFLAGS = $(AM_LDFLAGS) -export-dynamic $(FRAMEWORKADD) -static $(smacqq_DEPENDENCIES)  -L$(top_builddir)/libsmacq 
smacqq_LDADD = -lsmacq_modules -ldts_modules -lltdl -lsmacq
#CXXLD=--tag=CXX $(top_srcdir)/misc/g++

#smacqq_LDFLAGS = $(AM_LDFLAGS) -export-dynamic $(BUILTINS) $(FRAMEWORKADD) -static -v -Wl,-static,-lglib-2.0,-Bdynamic
#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
