# automake mangles the compilation and linking options, so the pysmacq module needs to be compiled
# mostly manually.

all-local: pysmacq 

BUILTINS = -export-dynamic -dlpreopen $(top_builddir)/libsmacq/libsmacq.la 

# Set this to your python library path
PYTHON_INCLUDES = -I/home/pflarr/include/python2.4
#PYTHON_INCLUDES = -I/usr/include/python2.4

PYSMACQ_INCLUDES = -I$(top_srcdir)/include/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include $(PYTHON_INCLUDES)
PYSMACQ_FLAGS = $(BUILTINS) -shared -Wl,-h -Wl,pysmacq.so
PYSMACQ_LIBS = -lltdl -lboost_python 
PYSMACQ_SOURCE = $(top_srcdir)/pysmacq/pysmacq.cpp

pysmacq: 
	libtool --mode=compile $(CXX) -c $(PYSMACQ_INCLUDES) $(PYSMACQ_SOURCE)
	libtool --mode=link $(CXX) $(PYSMACQ_FLAGS) $(PYSMACQ_LIBS) pysmacq.lo -o pysmacq.so

