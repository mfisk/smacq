# automake mangles the compilation and linking options, so the pysmacq module needs to be compiled
# mostly manually.

EXTRA_DIST=smacq.py
noinst_LTLIBRARIES=libpysmacq.la

libpysmacq_la_SOURCES=pysmacq.cpp
#libpysmacq_la_LIBADD = $(LIBS_PYTHON) @GLIB_LIBS@ -lboost_python $(top_builddir)/libsmacq/libsmacq.la
libpysmacq_la_LIBADD = $(top_builddir)/libsmacq/libsmacq.la -lboost_python  -lltdl
libpysmacq_la_LDFLAGS= -module $(AM_LDFLAGS) -rpath /usr/local/lib -shrext '.so' -export_dynamic 
libpysmacq_la_CPPFLAGS=$(AM_CPPFLAGS) $(CFLAGS_PYTHON) 

