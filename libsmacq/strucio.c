#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE
#endif

#define INDEXFILE ".strucio_index"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <zlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <strucio.h>

struct strucio {
  char * oneshot_filename;

  /* Call-backs */
  strucio_newfile_fn * newfile_fn;
  void * newfile_data;
  strucio_nextfilename_fn * nextfilename_fn;
  void * nextfilename_data;
  enum strucio_read_type read_type;

  /* Currenly opened file */
  char * filename;
  FILE * fh;

  /* For zlib */
  int use_gzip;
  gzFile * gzfh;

  /* For mmap */
  void * mmap;
  void * mmap_current;
  void * mmap_end;

  /* For rotation */
  long long outputleft;
  long long maxfilesize;
  int suffix;

  /* For filelist_args */
  int argc;
  char ** argv;

  /* For index lookup */
  long long lower_bound, upper_bound;
  char * indexroot;
  FILE * index_fh;
};

struct strucio * strucio_init() {
  struct strucio * rdr = calloc(1, sizeof(struct strucio));
  return rdr;
}

void strucio_set_use_gzip(struct strucio * r, int use_gzip) {
  r->use_gzip = use_gzip;
}

void strucio_register_newfile(struct strucio * r, strucio_newfile_fn * fn, void * data) {
  r->newfile_fn = fn;
  r->newfile_data = data;
}  

void strucio_register_filelist(struct strucio * r, strucio_nextfilename_fn * fn, void * data) {
  r->nextfilename_fn = fn;
  r->nextfilename_data = data;
  r->oneshot_filename = NULL;
}

void strucio_register_file(struct strucio * r, char * filename) {
  r->nextfilename_fn = NULL;
  r->oneshot_filename = filename;
}

void strucio_set_read_type(struct strucio * r, enum strucio_read_type read_type) {
  r->read_type = read_type;
}

void strucio_set_rotate(struct strucio * r, long long size) {
  r->maxfilesize = size;
}

void filelist_bounded_init(struct strucio * r) {
  char * indexfile = alloca(strlen(r->indexroot) + strlen(INDEXFILE) + 2);
  sprintf(indexfile, "%s/%s", r->indexroot, INDEXFILE);

  r->index_fh = fopen(indexfile, "r");
  if (!r->index_fh) {
 	fprintf(stderr, "No index file %s\n", indexfile);
	exit(-1); 
	/* Sould probably brute-force it instead */
  }
}

static char * filelist_bounded(struct strucio * r, void * user) {
  long long key, offset;
  char file[4096];

  if (!r->index_fh) {
	filelist_bounded_init(r);
  }

  while (1) {
	/* Find next matching line entry */
	int res = fscanf(r->index_fh, "%lld %s %lld", &key, file, &offset);
	if (res == -1) return NULL;

	assert(res == 3);
	/* fprintf(stderr, "Looking for %lld in file starting at %lld: %s\n", r->lower_bound, key, file); */
	if (key >= r->lower_bound && key <= r->upper_bound) {
		return strdup(file); /* memory leak */
        } 
  }
  return NULL; /* Shouldn't get here */
}

static char * filelist_stdin(struct strucio * r, void * user) {
  int res;
  char * filename = NULL;

  res = scanf("%4096s", filename);

  if (res == EOF) {
    return NULL;
  } else if (res != 1) {
    fprintf(stderr, "Error reading next filename from STDIN\n");
    return NULL;
  } else {
    return filename;
  }
}

static char * filelist_args(struct strucio * rdr, void * data) {
  if (! rdr->argc) {
    //fprintf(stderr, "No more files to read from\n");
    return(NULL);
  } else {
    char * filename = rdr->argv[0];
    rdr->argc--; rdr->argv++;
    return filename;
  }
}

void strucio_register_filelist_bounded(struct strucio * r, char * root, long long lower, long long upper) {
  r->indexroot = root;
  r->lower_bound = lower;
  r->upper_bound = upper;
  strucio_register_filelist(r, filelist_bounded, NULL);
}

void strucio_register_filelist_stdin(struct strucio * r) {
  strucio_register_filelist(r, filelist_stdin, NULL);
}

void strucio_register_filelist_args(struct strucio * r, int argc, char ** argv) {
  r->argc = argc;
  r->argv = argv;
  strucio_register_filelist(r, filelist_args, NULL);
}


static inline void * read_mmap(struct strucio * rdr, int len) {
      void * current = rdr->mmap_current;
      rdr->mmap_current += len;

      if (rdr->mmap_current > rdr->mmap_end) {
	if ((rdr->mmap_current - len) > rdr->mmap_end) {
	  fprintf(stderr, "structured read: premature end of mmapped file\n");
	}
	return NULL;
      }

      return current;
}

static inline void * read_current_file(struct strucio * rdr, void * buf, int len) {
      int retval;

      if (rdr->mmap) {
	void * map = read_mmap(rdr, len);
	if (!map) return map;
	memcpy(buf, map, len);
	return buf;
      } else if (rdr->gzfh) {
	retval = gzread(rdr->gzfh, buf, len);
	/* fprintf(stderr, "gzread returned %d\n", retval); */
	return( (retval) ? buf : NULL );
      } else if (rdr->fh) {
	retval = fread(buf, len, 1, rdr->fh);
	//fprintf(stderr, "fread returned %d on length %u\n", retval, len);
	return( (retval==1) ? buf : NULL );
      } else {
	/* Need to open a file */
	return(NULL);
      }
}


static inline void close_file(struct strucio* rdr) {
  if (rdr->gzfh) {
    gzclose(rdr->gzfh);
    rdr->gzfh = NULL;
  }
  
  if (rdr->fh) {
    fclose(rdr->fh);
    rdr->fh = NULL;
  }

  if (rdr->mmap) {
    rdr->mmap = NULL;
  }
}

/* Read from this file or the next one */
/* Return 1 on success, 0 on failure */
static inline void * strucio_read_multi(struct strucio * rdr, void * buf, int len, enum strucio_read_type read_type) {
  void * result;

  if (read_type == MMAP && !rdr->mmap) {
    /* Can't fulfil MMAP request */
    fprintf(stderr, "strucio error: MMAP read requested of non-mmapped file\n");
    return NULL;
  }
    
  while (1) {
    if (rdr->mmap && read_type != COPY) {
      result = read_mmap(rdr, len);
    } else {
      result = read_current_file(rdr, buf, len);
    }

    if (result) return result;
      
    /* Need to try another file */
    close_file(rdr);
    if (!strucio_open(rdr)) {
      return NULL;
    } 
  }
}

void * strucio_read_copy(struct strucio * rdr, void * buf, int len) {
  return strucio_read_multi(rdr, buf, len, COPY);
}

void * strucio_read_mmap(struct strucio * rdr, int len) {
  return strucio_read_multi(rdr, NULL, len, MMAP);
}

void * strucio_read(struct strucio * rdr, void * buf, int len) {
  return strucio_read_multi(rdr, buf, len, EITHER);
}

static inline int open_filename(struct strucio * rdr, char * filename) {
  int try_mmap = 0;

  if ((!filename) || (!strcmp(filename, "-"))) {
    if (rdr->use_gzip) {
	    rdr->gzfh = gzdopen(0, "rb");
    } else {
	    rdr->fh = fdopen(0, "r");
    }
  } else {
    if (rdr->use_gzip) {
	    rdr->gzfh = gzopen(filename, "rb");
    } else {
	    rdr->fh = fopen(filename, "r");
	    try_mmap = (rdr->read_type != COPY);
    }
  }

  if (!rdr->fh && !rdr->gzfh) {
    perror("structured open");
    exit(-1);
  }

  if (try_mmap) {
    struct stat stats;
    fstat(fileno(rdr->fh), &stats);

    rdr->mmap = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE|MAP_NORESERVE, fileno(rdr->fh), 0);
    if ((int)rdr->mmap == -1) rdr->mmap = NULL;
    
    if (rdr->mmap) {
      rdr->mmap_end = rdr->mmap + stats.st_size;
      rdr->mmap_current = rdr->mmap + ftell(rdr->fh);
    } else {
      fprintf(stderr, "structured: Error memory mapping file.  Will use stream I/O.\n");
    }
  }
  return(1); /* success */
}

/* Return 1 on success */
int strucio_open(struct strucio * rdr) {
  char * filename;

  if (rdr->oneshot_filename) {
    filename = rdr->oneshot_filename;
    rdr->oneshot_filename = NULL;
  } else if (rdr->nextfilename_fn) {
    filename = rdr->nextfilename_fn(rdr, rdr->nextfilename_data);
  } else {
    return -1;
  }

  if (!filename) return 0;

  if (!open_filename(rdr, filename)) return 0;

  if (rdr->newfile_fn) {
    return rdr->newfile_fn(rdr, rdr->newfile_data);
  }
   
  return 1;
}


/* -1 iff error */
static int strucio_openwrite(struct strucio * rdr) {
  char sufbuf[256];
  char * filename;
  
  if (! rdr->fh) {
    rdr->filename = rdr->oneshot_filename;
    fprintf(stderr, "Output will be placed in structured file %s\n", rdr->filename);
    
    if (rdr->maxfilesize) {
      snprintf(sufbuf, 256, "%s.%02d", rdr->filename, rdr->suffix);
      filename = sufbuf;
      rdr->suffix++;
      
      rdr->outputleft = rdr->maxfilesize;
    } else {
      filename = rdr->filename;
    }
    
    if (!strcmp(filename, "-")) {
      rdr->fh = stdout;
    } else {
      rdr->fh = fopen(filename, "w");
    }
    assert(rdr->fh);

    if (rdr->newfile_fn) {
      return rdr->newfile_fn(rdr, rdr->newfile_data);
    }
  
  }

  return 0;
}

int strucio_write(struct strucio * rdr, void * record, int len) {
  assert(record);

  rdr->outputleft -= len;

  //fprintf(stderr, "%lld left\n", rdr->outputleft);
  if (rdr->maxfilesize && (rdr->outputleft <= 0)) {
    fclose(rdr->fh);
    rdr->fh = NULL;
  }

  if (0 != strucio_openwrite(rdr)) 
    return -1;
  
  if (1 != fwrite(record, len,  1, rdr->fh)) {
    perror("structured write");
    return -1;
  }
	
  return 0;
}

void strucio_close(struct strucio * rdr) {
  if (rdr->fh) { fclose(rdr->fh);}
  if (rdr->gzfh) { gzclose(rdr->gzfh); }
  free(rdr);
}
