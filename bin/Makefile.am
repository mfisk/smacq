INCLUDES = -I$(top_srcdir)/include @GLOBAL_INCLUDES@
CFLAGS = -DSMACQ_BUILD_DATE=\"`date '+%Y-%m-%d'`\"
BUILTINS = $(top_builddir)/libsmacq/libsmacq.la -dlopen self -dlopen $(top_builddir)/modules/libsmacq_modules.la -dlopen $(top_builddir)/types/libdts_modules.la

bin_PROGRAMS = smacqq #smacqq_static smacqq_dynamic #iterative-example
smacqq_DEPENDENCIES = $(top_builddir)/libsmacq/libsmacq.la $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la
smacqq_LDFLAGS = -export-dynamic -static $(BUILTINS) -lglib-2.0 -lgmodule-2.0 -lpthread
#smacqq_static_LDFLAGS = -export-dynamic -all-static $(BUILTINS)
#smacqq_dynamic_LDFLAGS = -export-dynamic -dynamic
