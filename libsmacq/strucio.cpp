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


FilelistBounded::FilelistBounded(char * indexroot, long long lower, long long upper) {
  this->lower_bound = lower;
  this->upper_bound = upper;

  char * indexfile = (char*)alloca(strlen(indexroot) + strlen(INDEXFILE) + 2);
  sprintf(indexfile, "%s/%s", indexroot, INDEXFILE);

  index_fh = fopen(indexfile, "r");
  if (!index_fh) {
 	fprintf(stderr, "No index file %s\n", indexfile);
	exit(-1); 
	/* Sould probably brute-force it instead */
  }
}

char * FilelistBounded::nextfilename() {
  long long key, offset;
  char file[4096];

  while (1) {
	/* Find next matching line entry */
	int res = fscanf(index_fh, "%lld %s %lld", &key, file, &offset);
	if (res == -1) return NULL;

	assert(res == 3);
	/* fprintf(stderr, "Looking for %lld in file starting at %lld: %s\n", lower_bound, key, file); */
	if (key >= lower_bound && key <= upper_bound) {
		return strdup(file); /* memory leak */
        } 
  }
  return NULL; /* Shouldn't get here */
}

char * FilelistStdin::nextfilename() {
  int res;
  char filename[4096];

  res = scanf("%4096s", filename);

  if (res == EOF) {
    return NULL;
  } else if (res != 1) {
    fprintf(stderr, "Error reading next filename from STDIN\n");
    return NULL;
  } else {
    return strdup(filename);
  }
}

FilelistArgs::FilelistArgs(int num, char** names) {
  strucio_argc = num;
  assert(num >= 0);
  strucio_argv = names;
}

char * FilelistArgs::nextfilename() {
  if (! strucio_argc) {
    //fprintf(stderr, "No more files to read from\n");
    return(NULL);
  } else {
    char * filename = strucio_argv[0];
    strucio_argc--; strucio_argv++;
    return filename;
  }
}

void Strucio::register_filelist_bounded(char * root, long long lower, long long upper) {
  newFilelist(new FilelistBounded(root, lower, upper));
}

void Strucio::register_filelist_stdin() {
  newFilelist(new FilelistStdin());
}

void Strucio::register_filelist_args(int argc, char ** argv) {
  newFilelist(new FilelistArgs(argc, argv));
}

void Strucio::set_use_gzip(int use_gzip) {
  this->use_gzip = use_gzip;
}

void Strucio::register_file(char * filename) {
  filelist = new FilelistOneshot(filename);
}

void Strucio::set_read_type(enum strucio_read_type read_type) {
  this->read_type = read_type;
}

void Strucio::set_rotate(long long size) {
  maxfilesize = size;
}

inline void * Strucio::get_mmap(int len) {
      unsigned char * current = mmap_current;
      mmap_current += len;

      if (mmap_current > mmap_end) {
	if ((mmap_current - len) > mmap_end) {
	  fprintf(stderr, "structured read: premature end of mmapped file\n");
	}
	return NULL;
      }

      return current;
}

inline void * Strucio::read_current_file(void * buf, int len) {
      int retval;

      if (mmap_current) {
	void * map = get_mmap(len);
	if (!map) return map;
	memcpy(buf, map, len);
	return buf;
      } else if (gzfh) {
	retval = gzread(gzfh, buf, len);
	/* fprintf(stderr, "gzread returned %d\n", retval); */
	return( (retval) ? buf : NULL );
      } else if (fh) {
	retval = fread(buf, len, 1, fh);
	//fprintf(stderr, "fread returned %d on length %u\n", retval, len);
	return( (retval==1) ? buf : NULL );
      } else {
	/* Need to open a file */
	return(NULL);
      }
}


inline void Strucio::close_file() {
  if (gzfh) {
    gzclose(gzfh);
    gzfh = NULL;
  }
  
  if (fh) {
    fclose(fh);
    fh = NULL;
  }

  if (mmap_current) {
    mmap_current = NULL;
  }
}

/* Read from this file or the next one */
/* Return 1 on success, 0 on failure */
void * Strucio::read_multi(void * buf, int len, enum strucio_read_type read_type) {
  void * result;

  if (read_type == MMAP && !mmap_current) {
    /* Can't fulfil MMAP request */
    fprintf(stderr, "strucio error: MMAP read requested of non-mmapped file\n");
    return 0;
  }
    
  while (1) {
    if (mmap_current && read_type != COPY) {
      result = get_mmap(len);
    } else {
      result = read_current_file(buf, len);
    }

    if (result) return result;
      
    /* Need to try another file */
    close_file();
    if (0 != this->open()) {
      return 0; /* error */
    } 
  }
}

inline int Strucio::open_filename(char * filename) {
  int try_mmap = 0;

  if ((!filename) || (!strcmp(filename, "-"))) {
    if (use_gzip) {
	    gzfh = gzdopen(0, "rb");
    } else {
	    fh = fdopen(0, "r");
    }
  } else {
    if (use_gzip) {
	    gzfh = gzopen(filename, "rb");
    } else {
	    fh = fopen(filename, "r");
	    try_mmap = (read_type != COPY);
    }
  }

  if (!fh && !gzfh) {
    perror("structured open");
    exit(-1);
  }

  if (try_mmap) {
    struct stat stats;
    fstat(fileno(fh), &stats);

    unsigned char * mmap_start = (unsigned char*)mmap(NULL, stats.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_NORESERVE, fileno(fh), 0);
    if ((int)mmap_start == -1) mmap_start = NULL;
    
    if (mmap_start) {
      mmap_end = mmap_start + stats.st_size;
      mmap_current = mmap_start + ftell(fh);
    } else {
      fprintf(stderr, "structured: Error memory mapping file.  Will use stream I/O.\n");
    }
  }
  return(1); /* success */
}

/* Return 0 on success */
int Strucio::open() {
  char * filename = filelist->nextfilename();
  if (!filename) return -1;

  if (!open_filename(filename)) return -1;

  newfile_hook();
   
  return 0;
}


/* -1 iff error */
/* 0 on success */
int Strucio::openwrite() {
  char sufbuf[256];
  char * filename;
  
  if (! fh) {
    filename = filelist->nextfilename();
    fprintf(stderr, "Output will be placed in structured file %s\n", filename);
    
    if (maxfilesize) {
      snprintf(sufbuf, 256, "%s.%02d", filename, suffix);
      filename = sufbuf;
      suffix++;
      
      outputleft = maxfilesize;
    } else {
      filename = filename;
    }
    
    if (!strcmp(filename, "-")) {
      fh = stdout;
    } else {
      fh = fopen(filename, "w");
    }
    assert(fh);

    newfile_hook();
  }

  return 0;
}

int Strucio::write(void * record, int len) {
  assert(record);

  outputleft -= len;

  //fprintf(stderr, "%lld left\n", outputleft);
  if (maxfilesize && (outputleft <= 0)) {
    fclose(fh);
    fh = NULL;
  }

  if (0 != Strucio::openwrite()) 
    return -1;
  
  if (1 != fwrite(record, len,  1, fh)) {
    perror("structured write");
    return -1;
  }
	
  return 0;
}

void Strucio::newFilelist(Filelist * fl) {
  if (this->filelist)
    delete this->filelist;
  this->filelist = fl;
}

Strucio::Strucio() :
  filename(NULL), fh(NULL), use_gzip(false), gzfh(NULL), outputleft(0), suffix(0),
  mmap_current(NULL), mmap_end(NULL), 
  filelist(new FilelistError()) // We're pure virtual
{ }

Strucio::~Strucio() {
  delete filelist;

  if (fh) { fclose(fh);}
  if (gzfh) { gzclose(gzfh); }
}

void Strucio::newfile_hook() {
  fprintf(stderr, "default newfile_hook() called\n");
  return;
}
