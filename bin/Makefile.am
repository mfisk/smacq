INCLUDES = -I$(top_srcdir)/include @GLOBAL_INCLUDES@
BUILTINS = -dlopen self -dlopen $(top_builddir)/modules/libsmacq_modules.la -dlopen $(top_builddir)/types/libdts_modules.la -dlopen $(top_builddir)/libsmacq/libsmacq.la
AM_CFLAGS = @CFLAGS@ -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\"

bin_PROGRAMS = smacqq pgsmacqd #smacqq_static smacqq_dynamic #iterative-example

AM_LDFLAGS = @LDFLAGS@ $(BUILTINS) -export-dynamic -static 
LDADD = $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/libsmacq/libsmacq.la $(top_builddir)/types/libdts_modules.la -lpthread @LDADDS@
#DEPENDENCIES = $(top_builddir)/libsmacq/libsmacq.la $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la

#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
