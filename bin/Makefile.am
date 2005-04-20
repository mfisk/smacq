BUILTINS = -dlopen self -dlopen $(top_builddir)/modules/libsmacq_modules.la -dlopen $(top_builddir)/types/libdts_modules.la -dlopen $(top_builddir)/libsmacq/libsmacq.la
AM_CXXFLAGS = @CFLAGS@ -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\"

bin_PROGRAMS = smacqq #pgsmacqd #smacqq_static smacqq_dynamic #iterative-example
CCLD=$(CXX)  #Use C++ even if daemon code is plain C (libraries are C++)

smacqq_SOURCES = smacqq.cpp
#EXTRA_DIST = pgsmacqd.h

AM_LDFLAGS = @LDFLAGS@ $(BUILTINS) -export-dynamic -static
LDADD = $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/libsmacq/libsmacq.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libgnu/libgnu.la  
#DEPENDENCIES = $(top_builddir)/libsmacq/libsmacq.la $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la

#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
