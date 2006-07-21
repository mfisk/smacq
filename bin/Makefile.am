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
LIBTOOL = PATH=$(top_srcdir)/misc:$$PATH @LIBTOOL@ 
#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
