#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <smacq.h>
#include <zlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

enum file_read_type { EITHER, COPY, MAP };
static int open_file(struct state * state);
static inline void * read_current_file(struct state * state, void * buf, int len, enum file_read_type read_type);

static struct smacq_options options[] = {
  {"s", {uint32_t:0}, "Maximum output file size (MB)", SMACQ_OPT_TYPE_UINT32},
  {"l", {boolean_t:0}, "Read list of input files from STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"z", {boolean_t:0}, "Use gzip compression", SMACQ_OPT_TYPE_BOOLEAN},
  {"M", {boolean_t:0}, "Disable memory-mapped I/O", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  dts_object * datum;	
  smacq_environment * env;
  int argc;
  char ** argv;

  /* Booleans */
  int gzip;
  int hdr_size;
  int avoid_mmap;

  void * mmap;
  void * mmap_current;
  void * mmap_end;

  gzFile * gzfh;
  FILE * fh;

  long long outputleft;
  unsigned long long maxfilesize;
  int suffix;
  int file_list;
  char * filename;

  int cflow_type;
  int cflow_type_size;
};


#define SWAPLONG(y) \
((((y)&0xff)<<24) | (((y)&0xff00)<<8) | (((y)&0xff0000)>>8) | (((y)>>24)&0xff))
#define SWAPSHORT(y) \
        ( (((y)&0xff)<<8) | ((u_short)((y)&0xff00)>>8) )

#define TCPDUMP_MAGIC 0xa1b2c3d4
#define TCPDUMP_MAGIC_NEW 0xa1b2cd34

static inline void * read_current_file(struct state * state, void * buf, int len, enum file_read_type read_type) {
    if (state->mmap) {
      void * current = state->mmap_current;
      state->mmap_current += len;

      if (state->mmap_current > state->mmap_end) {
	if ((state->mmap_current - len) > state->mmap_end) {
	  fprintf(stderr, "cflow: premature end of mmapped file\n");
	}
	return NULL;
      }
      
      if (read_type == COPY) {
	memcpy(buf, current, len);
	return buf;
      } else {
	return current;
      }
    } else {
      int retval;
      assert(read_type != MAP);
      
      if (state->gzfh) {
	retval = gzread(state->gzfh, buf, len);
	//fprintf(stderr, "gzread returned %d\n", retval);
	return( (retval==1) ? buf : NULL );
      } else {
	retval = fread(buf, len, 1, state->fh);
	//fprintf(stderr, "fread returned %d on length %u\n", retval, len);
	return( (retval==1) ? buf : NULL );
      }
    }
}


static inline void close_file(struct state* state) {
  if (state->gzfh) {
    gzclose(state->gzfh);
    state->gzfh = NULL;
  }
  
  if (state->fh) {
    fclose(state->fh);
    state->fh = NULL;
  }

  if (state->mmap) {
    state->mmap = NULL;
  }
}

/* Read from this file or the next one */
static inline void * read_file(struct state * state, void * buf, int len, enum file_read_type read_type) {
  void * result;

  while (1) {
    result = read_current_file(state, buf, len, read_type);
    if (result) return result;

    close_file(state);
    if (!open_file(state)) {
      return NULL;
    } 
  }

}


static inline int open_filename(struct state * state, char * filename) {
  int try_mmap = 0;

  if ((!filename) || (!strcmp(filename, "-"))) {
    if (state->gzip) {
	    state->gzfh = gzdopen(0, "rb");
    } else {
	    state->fh = fdopen(0, "r");
    }
  } else {
    if (state->gzip) {
	    state->gzfh = gzopen(filename, "rb");
    } else {
	    state->fh = fopen(filename, "r");
	    try_mmap = ! state->avoid_mmap;
    }
  }

  if (!state->fh && !state->gzfh) {
    perror("cflow open");
    exit(-1);
  }
  
  if (try_mmap) {
    struct stat stats;
    fstat(fileno(state->fh), &stats);

    state->mmap = mmap(NULL, stats.st_size, PROT_READ, MAP_SHARED, fileno(state->fh), 0);
    if ((int)state->mmap == -1) state->mmap = NULL;
    
    if (state->mmap) {
      state->mmap_end = state->mmap + stats.st_size;
      state->mmap_current = state->mmap + ftell(state->fh);
    } else {
      fprintf(stderr, "cflow: Error memory mapping file.  Will use stream I/O.\n");
    }
  }

  fprintf(stderr, "cflow: Opening %s for read ( ", filename);

  if (state->mmap) fprintf(stderr, "memory-mapped ");

  fprintf(stderr, ")\n");

  return(1); /* success */
}

static int open_file(struct state * state) {
  char * filename = "";
  char next_file[4096];

  if (state->file_list) {
	  int res;
	  filename = next_file;
	  res = scanf("%4096s", filename);
	  if (res == EOF) {
		  return(0);
	  } else if (res != 1) {
		  fprintf(stderr, "Error reading next filename from STDIN\n");
		  return(0);
	  }
  } else if (! state->argc) {
    //fprintf(stderr, "No more files to read from\n");
    return(0);
  } else {
    filename = state->argv[0];
    state->argc--; state->argv++;
  }

  return open_filename(state, filename);
}

static smacq_result cflow_produce(struct state * state, const dts_object ** datump, int * outchan) {
  const dts_object * datum;

  datum = smacq_alloc(state->env, state->cflow_type_size, state->cflow_type);
  datum->data = read_file(state, dts_getdata(datum), state->cflow_type_size, EITHER);

  if (!datum->data) {
	dts_decref(datum);
	return SMACQ_END;
  }

  *datump = datum;

  return SMACQ_PASS|SMACQ_PRODUCE;
}

static smacq_result cflow_shutdown(struct state * state) {
	close_file(state);
	free(state);

	return 0;
}

static smacq_result cflow_init(struct smacq_init * context) {
  struct state * state;

  smacq_requiretype(context->env, "packet");

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    smacq_opt output, size, list, gzip, avoid_mmap;

    struct smacq_optval optvals[] = {
      { "w", &output}, 
      { "s", &size}, 
      { "l", &list}, 
      { "z", &gzip}, 
      { "M", &avoid_mmap}, 
      {NULL, NULL}
    };
    output.uint32_t = 0;
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    
    state->filename = output.string_t;
    state->file_list = list.boolean_t;
    state->maxfilesize = size.uint32_t * 1024 * 1024;
    state->outputleft = 1024*1024;
    state->gzip = gzip.boolean_t;
    state->avoid_mmap = avoid_mmap.boolean_t;
  }
  
  open_file(state);

  if (!context->isfirst) {
    fprintf(stderr, "cflow module must be at beginning of dataflow\n");
    exit(-1);
  }

  state->cflow_type = smacq_requiretype(context->env, "cflow");
  state->cflow_type_size = dts_type_size(context->env->types, state->cflow_type);

  return 0;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_cflow_table = {
  produce: &cflow_produce, 
  init: &cflow_init,
  shutdown: &cflow_shutdown
};

