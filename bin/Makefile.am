BUILTINS = -dlopen self -dlopen $(top_builddir)/modules/libsmacq_modules.la -dlopen $(top_builddir)/types/libdts_modules.la -dlopen $(top_builddir)/libsmacq/libsmacq.la

bin_PROGRAMS = smacqq 

if GASNET
smacqq_SOURCES = psmacqq.cpp 
smacqq_CPPFLAGS = $(AM_CPPFLAGS) -fpermissive -Wno-error -Wno-unused # GASnet code isn't very pedantic
else
smacqq_CPPFLAGS = $(AM_CPPFLAGS)
smacqq_SOURCES = smacqq.cpp 
endif

smacqq_LDFLAGS = $(AM_LDFLAGS) $(BUILTINS) -export-dynamic -static $(FRAMEWORKADD)
LDADD = $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/libsmacq/libsmacq.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libgnu/libgnu.la  

#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
