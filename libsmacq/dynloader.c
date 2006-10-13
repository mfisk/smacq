#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ltdl.h>
#include <assert.h>
#include <string.h>

struct preload_entry {
	void * ptr;
	char * sym;
};
#include "preload.h"

static void * smacq_try_dlfindsym_preload(char * format, char * sym) {
    char trysym[1024];
    snprintf(trysym, 1023, format, sym);
    struct preload_entry *p;
    for (p = preloads; p->sym; p++) {
    	//fprintf(stderr, "Comparing %s to %s\n", trysym, p->sym);
	if (!strcmp(p->sym, trysym)) {
		return p->ptr;
	}
    }
    fprintf(stderr, "Symbol %s not preloaded\n", trysym);
    return NULL;
}

static void * smacq_try_dlfindsym(lt_dlhandle gmodule, char * format, char * sym) {
    char trysym[1024];
    void * modtable;

    snprintf(trysym, 1023, format, sym);

        // Linux has no leading underscore
	modtable = lt_dlsym(gmodule, trysym);
//    printf("First attempt: %p, %s, %p\n", gmodule, trysym, modtable);

	if (!modtable) {
		// Darwin has leading underscore
    		char trysym2[1024];

    		snprintf(trysym2, 1023, "_%s", trysym);
//            fprintf(stderr, "Info: looking for %s/%s:%s in %p\n", format, sym, trysym2, gmodule);
    		modtable = lt_dlsym(gmodule, trysym2);
	}

//    fprintf(stderr, "Info: found %s/%s at %p\n", format, sym, modtable);
	return modtable;
}

void * smacq_find_module(lt_dlhandle* gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym) {
    void * modtable = NULL;

    assert(gmodulep);
    assert(sym);

    // Try preloaded symbols first
    modtable = smacq_try_dlfindsym_preload(symformat, sym);
    if (modtable) {
	fprintf(stderr, "Found %s in preloads\n", sym);
	return modtable;
    }

    lt_dlinit();

/*
    void * self = lt_dlopen(NULL);
    if (!self) {
	fprintf(stderr, "Warning: %s\n", lt_dlerror());
    } else {
        modtable = smacq_try_dlfindsym(self, symformat, sym);

        if (modtable) {
    	  //printf("dlfindsym found %p\n", modtable);
	  return modtable;
        }
    }
*/

    // If not in self, then try opening a module 
    char modfile[256];
    char * path = getenv(envvar);

    // Build shared library location
    if (!path) path = envdefault;

    snprintf(modfile, 256, modformat, path, sym);
    //printf("Before: %p\n", *gmodulep);
    *gmodulep = lt_dlopen(modfile);
    if (! *gmodulep ) {
           //fprintf(stderr, "%s (%s,%s,%s->%s) (Need to set SMACQ_HOME?)\n", sym, modformat, path, sym, modfile);
           return NULL;
    }

    modtable = smacq_try_dlfindsym(*gmodulep, symformat, sym);

    if (modtable) {
            return modtable;
    }
    
    fprintf(stderr, "Error: unable to find symbol %s\n", sym);
    return NULL;
}


