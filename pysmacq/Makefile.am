EXTRA_DIST=smacq.py
lib_LTLIBRARIES=libpysmacq.la

#SHELL=PATH=$(top_srcdir)/misc:$$PATH bash

libpysmacq_la_SOURCES=pysmacq.cpp ../libsmacq/preload.h #$(top_builddir)/types/libdts_modules.la $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/modules/libsmacq.la
libpysmacq_la_DEPENDENCIES = $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libsmacq/libsmacq.la 
#libpysmacq_la_LIBADD = -lboost_python -lltdl -lstdc++ @GLIB_LIBS@ $(LIBS_PYTHON) $(libpysmacq_la_DEPENDENCIES) 
#libpysmacq_la_LIBADD = -lsmacq_modules -ldts_modules -lsmacq -lltdl @LIBDB_CXX_LIBS@ -lboost_python $(LIBS_PYTHON) @GDA_LIBS@  @GLIB_LIBS@
libpysmacq_la_LIBADD = $(libpysmacq_la_DEPENDENCIES) $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la -lsmacq_modules -ldts_modules -lsmacq @LIBDB_CXX_LIBS@ -lboost_python $(LIBS_PYTHON) $(AM_LIBADD) @GDA_LIBS@ @GLIB_LIBS@ -lltdl 
libpysmacq_la_LDFLAGS= -module $(AM_LDFLAGS) -rpath $(LIBINST_PYTHON) -shrext '.so' -L$(top_builddir)/modules -L$(top_builddir)/types -L$(top_builddir)/libsmacq  
libpysmacq_la_CPPFLAGS=$(AM_CPPFLAGS) $(CFLAGS_PYTHON) 


