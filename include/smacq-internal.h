#ifndef LIBSMACQ_INTERNAL_H
#define LIBSMACQ_INTERNAL_H

BEGIN_C_DECLS
DtsObject _smacq_alloc(int size, int type);
END_C_DECLS

#include <gmodule.h>
#include <glib.h>

#define RING_EOF ((void*)-1)

struct smacq_module_ops {
  smacq_constructor_fn * constructor;
};

BEGIN_C_DECLS

void * smacq_find_module(GModule ** gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym);
void smacq_start_threads(SmacqGraph *);

END_C_DECLS

/*
 * Interace to buffer system 
 */
#include <buffer-inline.c>

#endif

