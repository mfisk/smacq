#include <flow-internal.h>
#include <stdio.h>
#include <dlfcn.h>

static inline void * smacq_try_dlsym(void * module, GModule * gmodule, char * trysym) {

	if (module) {
    		return dlsym(module, trysym);
	} else {
		void * modtable;
		assert(gmodule);
		if (g_module_symbol(gmodule, trysym, &modtable)) {
			return modtable;
		} else {
			return NULL;
		}
	}
}

static void * smacq_try_dlfindsym(void * module, GModule * gmodule, char * format, char * sym) {
    char trysym[1024];
    void * modtable;

    snprintf(trysym, 1023, format, sym);

        // Linux has no leading underscore
	modtable = smacq_try_dlsym(module, gmodule, trysym);

	if (!modtable) {
		// Darwin has leading underscore
    		char trysym2[1024];

    		snprintf(trysym2, 1023, "_%s", trysym);
    //fprintf(stderr, "Info: looking for %s/%s:%s in %p\n", format, sym, trysym2, module);
    		modtable = smacq_try_dlsym(module, gmodule, trysym2);
	}

    //fprintf(stderr, "Info: found %s/%s at %p\n", format, sym, modtable);
	return modtable;
}

void * smacq_find_module(GModule ** gmodulep, char * envvar, char * envdefault, char * modformat, char * symformat, char * sym) {
    void * self;
    void * modtable = NULL;

    assert(sym);

    self = dlopen(NULL, RTLD_NOW);
    if (!self) {
	fprintf(stderr, "Warning: %s\n", dlerror());
    } else {
    	modtable = smacq_try_dlfindsym(self, NULL, symformat, sym);
    }

    if (modtable) {
	return modtable;
    } else {
        /* If not in self, then try opening a module */
    	char modfile[256];
    	char * path = getenv(envvar);

    	assert(g_module_supported());

    	// Build shared library location
    	if (!path) path = envdefault;

    	snprintf(modfile, 256, modformat, path, sym);

        if (! (*gmodulep = g_module_open(modfile, 0))) {
           //fprintf(stderr, "%s (%s,%s,%s->%s): %s (Need to set SMACQ_HOME?)\n", sym, modformat, path, sym, modfile, g_module_error());
           return NULL;
        }

    	modtable = smacq_try_dlfindsym(NULL, *gmodulep, symformat, sym);

	if (modtable) {
		return modtable;
	}
    }

    //fprintf(stderr, "Error: unable to find symbol %s\n", sym);
    return NULL;
}


