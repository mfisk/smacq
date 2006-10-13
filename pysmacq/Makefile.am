# automake mangles the compilation and linking options, so the pysmacq module needs to be compiled
# mostly manually.

EXTRA_DIST=smacq.py
lib_LTLIBRARIES=libpysmacq.la

SHELL=env PATH=$(top_srcdir)/misc:$$PATH sh
libpysmacq_la_SOURCES=pysmacq.cpp
libpysmacq_la_LIBADD = -lboost_python -lltdl -lstdc++ @GLIB_LIBS@ $(LIBS_PYTHON) -lsmacq -lsmacq_modules -ldts_modules
libpysmacq_la_LDFLAGS= -module $(AM_LDFLAGS) -rpath $(LIBINST_PYTHON) -shrext '.so' -export_dynamic -L$(top_builddir)/libsmacq -L$(top_builddir)/modules -L$(top_builddir)/types
libpysmacq_la_CPPFLAGS=$(AM_CPPFLAGS) $(CFLAGS_PYTHON) 

