INCLUDES = -I$(top_srcdir)/include @GLOBAL_INCLUDES@
BUILTINS = -dlopen self -dlopen $(top_builddir)/modules/libsmacq_modules.la -dlopen $(top_builddir)/types/libdts_modules.la
CFLAGS = -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\"

bin_PROGRAMS = smacqq pgsmacqd #smacqq_static smacqq_dynamic #iterative-example

LDFLAGS = $(BUILTINS) -export-dynamic -static 
LDADD = $(top_builddir)/libsmacq/libsmacq.la -lglib-2.0 -lgmodule-2.0 -lpthread
DEPENDENCIES = $(top_builddir)/libsmacq/libsmacq.la $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la

#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
