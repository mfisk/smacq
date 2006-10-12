# automake mangles the compilation and linking options, so the pysmacq module needs to be compiled
# mostly manually.

EXTRA_DIST=smacq.py
noinst_LTLIBRARIES=libpysmacq.la

CXXLD=$(top_srcdir)/misc/g++
libpysmacq_la_SOURCES=pysmacq.cpp
libpysmacq_la_LIBADD = -lboost_python -lltdl -lstdc++ @GLIB_LIBS@ $(LIBS_PYTHON) $(top_builddir)/libsmacq/libsmacq.la
libpysmacq_la_LDFLAGS= -module $(AM_LDFLAGS) -rpath /usr/local/lib -shrext '.so' -export_dynamic #-dlpreopen $(top_builddir)/libsmacq/libsmacq.la 
libpysmacq_la_CPPFLAGS=$(AM_CPPFLAGS) $(CFLAGS_PYTHON) 

