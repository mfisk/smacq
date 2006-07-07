#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
#endif

#include <stdio.h>
#include <glib.h>

#define INDEXFILE ".strucio_index"

/// A pure virtual base for classes that return filenames.
class Filelist {
 public:
  virtual void nextfilename(char * buf, int len) = 0;
  virtual ~Filelist() {};
};

/// Return filenames from an index file.
class FilelistBounded : public Filelist {
 public:
  FilelistBounded(char * root, long long lower, long long upper);
  void nextfilename(char *, int);
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
class FilelistConstant : public Filelist {
 public:
  FilelistConstant(char * filename) : file(filename) {;}

  void nextfilename(char * filename, int len) { 
     if (this->file) {
	g_strlcpy(filename, this->file, len);
     }
  }

 protected:
  char * file;
};

/// Return a single filename.
class FilelistOneshot : public Filelist {
 public:
  FilelistOneshot(char * filename) { this->file = filename; }
  void nextfilename(char * filename, int len) { 
     if (this->file) {
	g_strlcpy(filename, this->file, len);
     	this->file = NULL;
     } else {
	filename[0] = '\0';
     }
  }

 protected:
  char * file;

};

/// Return file names from STDIN.
class FilelistStdin : public Filelist {
 public:
  void nextfilename(char *, int);
};

/// Return file names from an argument vector.
class FilelistArgs : public Filelist {
 public:
  FilelistArgs(int, char **);
  void nextfilename(char *, int);

 protected:
  int strucio_argc;
  char ** strucio_argv;
};

/// Never return a file name.
class FilelistError : public Filelist {
 public:
  void nextfilename(char * buf, int len) { buf[0] = '\0'; }
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

inline void FilelistBounded::nextfilename(char * file, int len) {
  long long key, offset;
  file[0] = '\0'; // Default result is empty

  while (1) {
	/* Find next matching line entry */
	///XXX: unchecked size
	int res = fscanf(index_fh, "%lld %s %lld", &key, file, &offset);
	if (res == -1) return;

	assert(res == 3);
	/* fprintf(stderr, "Looking for %lld in file starting at %lld: %s\n", lower_bound, key, file); */
	if (key >= lower_bound && key <= upper_bound) {
		return; 
        } 
  }
  return; /* Shouldn't get here */
}

inline void FilelistStdin::nextfilename(char * filename, int len) {
  int res;

  res = scanf("%4096s", filename);

  if (res == EOF) {
    filename[0] = '\0';
  } else if (res != 1) {
    fprintf(stderr, "Error reading next filename from STDIN\n");
    filename[0] = '\0';
  }
}

inline FilelistArgs::FilelistArgs(int num, char** names) {
  strucio_argc = num;
  assert(num >= 0);
  strucio_argv = names;
}

inline void FilelistArgs::nextfilename(char * file, int len) {
  if (! strucio_argc) {
    //fprintf(stderr, "No more files to read from\n");
    file[0] = '\0'; 
  } else {
    strncpy(file, strucio_argv[0], len);
    strucio_argc--; strucio_argv++;
  }
}

