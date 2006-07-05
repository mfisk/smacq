#include <stdio.h>
#include <zlib.h>

#include <Filelist.h>

enum strucio_read_type { EITHER, COPY, MMAP };

/// A file writer for structured data.
class StrucioWriter {
   public:
	StrucioWriter();
	virtual ~StrucioWriter();

	int write(void * record, int len);

	/* Called when new file opened */
	virtual void newfile_hook() { /* Base class does nothin. */ };

	/* Use one of the following: */
	void register_filelist_stdin();
	void register_filelist_args(int argc, char ** argv);
	void register_filelist_bounded(char * index_location, long long lower, long long upper);
	void register_file(char * filename);
	
	void set_rotate(long long size);
	void set_use_gzip(bool val) { use_gzip = val;}

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

	Filelist * filelist;
	void newFilelist(Filelist *);

	int openwrite();
	void close_file();
};


inline void StrucioWriter::register_filelist_bounded(char * root, long long lower, long long upper) {
  newFilelist(new FilelistBounded(root, lower, upper));
}

inline void StrucioWriter::register_filelist_stdin() {
  newFilelist(new FilelistStdin());
}

inline void StrucioWriter::register_filelist_args(int argc, char ** argv) {
  newFilelist(new FilelistArgs(argc, argv));
}

inline void StrucioWriter::register_file(char * filename) {
  filelist = new FilelistOneshot(filename);
}

inline void StrucioWriter::set_rotate(long long size) {
  maxfilesize = size;
}

inline void StrucioWriter::close_file() {
  if (fh) {
	fclose(fh);
	fh = NULL;
  }
}

/* -1 iff error */
/* 0 on success */
inline int StrucioWriter::openwrite() {
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

inline int StrucioWriter::write(void * record, int len) {
  assert(record);

  outputleft -= len;

  //fprintf(stderr, "%lld left\n", outputleft);
  if (fh && maxfilesize && (outputleft <= 0)) {
    close_file();
  }

  if (0 != StrucioWriter::openwrite()) 
    return -1;
  
  if (1 != fwrite(record, len,  1, fh)) {
    perror("structured write");
    return -1;
  }
	
  return 0;
}

inline void StrucioWriter::newFilelist(Filelist * fl) {
  if (this->filelist)
    delete this->filelist;
  this->filelist = fl;
}

inline StrucioWriter::StrucioWriter() :
  filename(NULL), outputleft(0), suffix(0),
  filelist(new FilelistError()) // We're pure virtual
{ }

inline StrucioWriter::~StrucioWriter() {
  close_file();
  delete filelist;
}

