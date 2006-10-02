#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ltdl.h>
#include <assert.h>
#include <string.h>

// Loads the module named sym by searching the list of preloaded symbols.
// This is only needed for when smacq is packaged as a python module, because the
// self gets mangled somehow in the process.  
// This should only be a temporary fix, either the mangling of "self" needs to be
// figured out and corrected, or the way modules are loaded should be changed.
static void * smacq_try_manual_findsym(char * format, char * sym) {
    char trysym[1024];
    // This definition is very finicky.  If this function seg faults, it's probably
    // because this external variable is no longer being declared/linked correctly.
    extern const lt_dlsymlist lt_preloaded_symbols[];
    int i = 0;
    
    snprintf(trysym, 1023, format, sym);
    
    // The last element in the list has a name and address pointer of 0
    while (lt_preloaded_symbols[i].name != 0 || lt_preloaded_symbols[i].address != 0) {
//        printf("Looking for module %s, looking at %s.\n", trysym, lt_preloaded_symbols[i].name);
        if (!strcmp(lt_preloaded_symbols[i].name,trysym) ) {
//            printf("Found module: %s using ghetto_loader\n", trysym);
            return lt_preloaded_symbols[i].address;
        }
        i++;
    }
    
//    printf("Could not find module: %s using ghetto_loader\n", trysym);
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
    void * self;
    void * modtable = NULL;

    assert(gmodulep);
    assert(sym);

    lt_dlinit();

    self = lt_dlopen(NULL);
    if (!self) {
	fprintf(stderr, "Warning: %s\n", dlerror());
    } else {
    	modtable = smacq_try_dlfindsym(self, symformat, sym);
    }

//    printf("dlfindsym found %p\n", modtable);

    // Try loading from self using the manual loader.
    if (modtable) {
        return modtable;
    } else {
        modtable = smacq_try_manual_findsym(symformat, sym);
    }

    if (modtable) {
	return modtable;
    } else {
        // If not in self, then try opening a module 
    	char modfile[256];
    	char * path = getenv(envvar);

    	// Build shared library location
    	if (!path) path = envdefault;

    	snprintf(modfile, 256, modformat, path, sym);
        //printf("Before: %p\n", *gmodulep);
        *gmodulep = lt_dlopen(modfile);
        if (! *gmodulep ) {
           fprintf(stderr, "%s (%s,%s,%s->%s) (Need to set SMACQ_HOME?, Yo)\n", sym, modformat, path, sym, modfile);
           return NULL;
        }

   // 	modtable = smacq_try_dlfindsym(*gmodulep, symformat, sym);


    if (modtable) {
        return modtable;
    }
    }
    
    fprintf(stderr, "Error: unable to find symbol %s\n", sym);
    return NULL;
}


