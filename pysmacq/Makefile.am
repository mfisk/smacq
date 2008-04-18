# automake mangles the compilation and linking options, so the pysmacq module needs to be compiled
# mostly manually.

EXTRA_DIST=smacq.py
lib_LTLIBRARIES=libpysmacq.la

#SHELL=PATH=$(top_srcdir)/misc:$$PATH bash

libpysmacq_la_SOURCES=pysmacq.cpp ../libsmacq/preload.h
libpysmacq_la_DEPENDENCIES = $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libsmacq/libsmacq.la
#libpysmacq_la_LIBADD = -lboost_python -lltdl -lstdc++ @GLIB_LIBS@ $(LIBS_PYTHON) $(libpysmacq_la_DEPENDENCIES) 
#libpysmacq_la_LIBADD = #-lsmacq_modules -ldts_modules -lsmacq -lltdl @LIBDB_CXX_LIBS@ -lboost_python @GLIB_LIBS@ $(LIBS_PYTHON) @GDA_LIBS@ 
libpysmacq_la_LIBADD = -lltdl @LIBDB_CXX_LIBS@ -lboost_python @GLIB_LIBS@ $(LIBS_PYTHON) @GDA_LIBS@ 
libpysmacq_la_LDFLAGS= -module $(AM_LDFLAGS) -rpath $(LIBINST_PYTHON) -shrext '.so' -export_dynamic $(libpysmacq_la_DEPENDENCIES)
libpysmacq_la_CPPFLAGS=$(AM_CPPFLAGS) $(CFLAGS_PYTHON) 


