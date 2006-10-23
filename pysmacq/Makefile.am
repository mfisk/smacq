# automake mangles the compilation and linking options, so the pysmacq module needs to be compiled
# mostly manually.

EXTRA_DIST=smacq.py
lib_LTLIBRARIES=libpysmacq.la

SHELL=env PATH=$(top_srcdir)/misc:$$PATH sh
libpysmacq_la_SOURCES=pysmacq.cpp
libpysmacq_la_DEPENDENCIES = $(top_builddir)/modules/libsmacq_modules.la $(top_builddir)/types/libdts_modules.la $(top_builddir)/libsmacq/libsmacq.la
libpysmacq_la_LIBADD = -lboost_python -lltdl -lstdc++ @GLIB_LIBS@ $(LIBS_PYTHON) $(libpysmacq_la_DEPENDENCIES) 
libpysmacq_la_LDFLAGS= -module $(AM_LDFLAGS) -rpath $(LIBINST_PYTHON) -shrext '.so' -export_dynamic 
libpysmacq_la_CPPFLAGS=$(AM_CPPFLAGS) $(CFLAGS_PYTHON) 

