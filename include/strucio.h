#include <stdio.h>
#include <zlib.h>

enum strucio_read_type { EITHER, COPY, MMAP };

/// A pure virtual base for classes that return filenames.
class Filelist {
 public:
  virtual char * nextfilename() = 0;
};


/// A file reader/writer for structured data.
class Strucio {
   public:
	Strucio();
	virtual ~Strucio();

	virtual void newfile_hook();

	int open();
	inline void * read(void * buf, int len);
	inline void * read_mmap(int len);
	inline void * read_copy(void * buf, int len);
	int write(void * record, int len);

	/* Use one of the following: */
	void register_filelist_stdin();
	void register_filelist_args(int argc, char ** argv);
	void register_filelist_bounded(char * index_location, long long lower, long long upper);
	void register_file(char * filename);
	
	void set_rotate(long long size);
	void set_read_type(enum strucio_read_type read_type);
	void set_use_gzip(int boolean);

   protected:
  	/* Currenly opened file */
  	char * filename;
  	FILE * fh;

  	/* For zlib */
  	int use_gzip;
  	gzFile gzfh;

  	/* For rotation */
  	long long outputleft;
  	long long maxfilesize;
  	int suffix;

  	/* For mmap */
  	unsigned char * mmap_current;
  	unsigned char * mmap_end;

	Filelist * filelist;
	void newFilelist(Filelist *);

	int openwrite();
	int open_filename(char*);
	void * get_mmap(int len);
	void * read_multi(void *, int, enum strucio_read_type);
	void * read_current_file(void *, int);
	void close_file();

  	enum strucio_read_type read_type;


};

inline void * Strucio::read_copy(void * buf, int len) {
  return read_multi(buf, len, COPY);
}
  
inline void * Strucio::read_mmap(int len) {
  return read_multi(NULL, len, MMAP);
}   
            
inline void * Strucio::read(void * buf, int len) {
  return read_multi(buf, len, EITHER);
}   
            
/// Return filenames from an index file.
class FilelistBounded : public Filelist {
 public:
  FilelistBounded(char * root, long long lower, long long upper);
  char * nextfilename();

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

