bin_PROGRAMS = smacqq #smacqq-static #iterative-example

BUILTINS = -export-dynamic -dlpreopen $(top_builddir)/libsmacq/libsmacq.la 

if GASNET
smacqq_SOURCES = psmacqq.cpp 
smacqq_CPPFLAGS = $(AM_CPPFLAGS) -fpermissive -Wno-error -Wno-unused # GASnet code isn't very pedantic
else
smacqq_SOURCES = smacqq.cpp 
smacqq_CPPFLAGS = $(AM_CPPFLAGS)
smacqq_static_SOURCES = smacqq.cpp 
endif

smacqq_DEPENDENCIES = $(top_builddir)/libsmacq/libsmacq.la
smacqq_LDFLAGS = $(AM_LDFLAGS) $(BUILTINS) $(FRAMEWORKADD) -static -lm
CXXLD=--tag=CXX $(top_srcdir)/misc/ldstatic g++
#smacqq_LDFLAGS = $(AM_LDFLAGS) $(BUILTINS) $(FRAMEWORKADD) -static -v -Wl,-static,-lglib-2.0,-Bdynamic
#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
