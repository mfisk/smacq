#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
#endif

#include <stdio.h>

#define INDEXFILE ".strucio_index"

/// A pure virtual base for classes that return filenames.
class Filelist {
 public:
  virtual char * nextfilename() = 0;
  virtual ~Filelist() {};
};

/// Return filenames from an index file.
class FilelistBounded : public Filelist {
 public:
  FilelistBounded(char * root, long long lower, long long upper);
  char * nextfilename();
  ~FilelistBounded() {
	if (index_fh) fclose(index_fh);
  }

 protected:
  char * indexfile;
  FILE * index_fh;
  long long lower_bound;
  long long upper_bound;
};

/// Return a single filename.
class FilelistOneshot : public Filelist {
 public:
  FilelistOneshot(char * filename) { this->file = filename; }
  char * nextfilename() { 
     char * filename = this->file;
     this->file = NULL;
     return filename;
  }

 protected:
  char * file;

};

/// Return file names from STDIN.
class FilelistStdin : public Filelist {
 public:
  char * nextfilename();
};

/// Return file names from an argument vector.
class FilelistArgs : public Filelist {
 public:
  FilelistArgs(int, char **);
  char * nextfilename();

 protected:
  int strucio_argc;
  char ** strucio_argv;
};

/// Never return a file name.
class FilelistError : public Filelist {
 public:
  char * nextfilename() { return NULL; }
};

inline FilelistBounded::FilelistBounded(char * indexroot, long long lower, long long upper) {
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

inline char * FilelistBounded::nextfilename() {
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

inline char * FilelistStdin::nextfilename() {
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

inline FilelistArgs::FilelistArgs(int num, char** names) {
  strucio_argc = num;
  assert(num >= 0);
  strucio_argv = names;
}

inline char * FilelistArgs::nextfilename() {
  if (! strucio_argc) {
    //fprintf(stderr, "No more files to read from\n");
    return(NULL);
  } else {
    char * filename = strucio_argv[0];
    strucio_argc--; strucio_argv++;
    return filename;
  }
}

