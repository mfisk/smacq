bin_PROGRAMS = smacqq #smacqq-static #iterative-example

BUILTINS = -dlopen $(top_builddir)/libsmacq/libsmacq.la

if GASNET
smacqq_SOURCES = psmacqq.cpp 
smacqq_CPPFLAGS = $(AM_CPPFLAGS) -fpermissive -Wno-error -Wno-unused # GASnet code isn't very pedantic
else
smacqq_SOURCES = smacqq.cpp 
smacqq_DEPENDENCIES = $(top_builddir)/libsmacq/libsmacq.la
smacqq_CPPFLAGS = $(AM_CPPFLAGS)
smacqq_static_SOURCES = smacqq.cpp 
endif

smacqq_LDFLAGS = $(AM_LDFLAGS) $(BUILTINS) $(FRAMEWORKADD)
smacqq_static_LDFLAGS = -all-static $(FRAMEWORKADD) $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -dynamic $(FRAMEWORKADD)
