#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
#endif

#include <fcntl.h>
#include <stdio.h>
#include <zlib.h>
#include <bzlib.h>
#include <DtsObject.h>
#include <SmacqModule.h>

/// Pure-virtual base class for input streams.
class StrucioStream {
   public: 
	/// Open a file and return a StrucioStream object for it.
        static StrucioStream * Open(const char * filename);

	/// Open a file specified in a DtsObject and return a StrucioStream object for it.
	static StrucioStream * Open(DtsObject fo);

	/// Called by modules that wish to defer file opening to fileModule.
	static void FileModule(SmacqModule::smacq_init * context);

	/// File read.
	virtual size_t Read(void * ptr, size_t bytes) = 0;

	/// Destructor (will close file).
	virtual ~StrucioStream() { fprintf(stderr, "Error, StrucioStream base destructor called\n"); }

	/// Construct an object.
	virtual DtsObject construct(DTS * dts, dts_typeid t) { 
		DtsObject o = dts->newObject(t);
		unsigned int size = o->getsize();
		unsigned int got;
		got = this->Read(o->getdata(), size);
		if (got != size) {
			o = NULL;
		}
		return o;
	}

	/// Construct an object.
	virtual DtsObject construct(DTS * dts, dts_typeid t, unsigned int size) { 
		DtsObject o = dts->newObject(t, size);
		assert(size == this->Read(o->getdata(), size));
		return o;
	}

   protected:
	/// Open a file descriptor.
   	//virtual StrucioStream(int fd, const char * mode) = 0;

	/// Open a file name.
   	//virtual StrucioStream(char * filename, const char * mode) = 0;

};


/// Input stream using libc FILE streams.
class fStrucioStream : public StrucioStream {
public:
   fStrucioStream(int fd, const char * mode = "rb") { fh = fdopen(fd, mode); } 
   fStrucioStream(char * fn, const char * mode = "rb") { fh = fopen(fn, mode); } 
   ~fStrucioStream() { fclose(fh); }
   size_t Read(void * ptr, size_t bytes) {
	return fread(ptr, 1, bytes, fh);
   }
   // XXX. this class could use mmap to implement construct() methods.

private:
   FILE * fh;
};

/// Input stream using libbz2 compression
class bzStrucioStream : public StrucioStream {
public:
   bzStrucioStream(int fd, const char * mode = "rb") { fh = BZ2_bzdopen(fd, mode); } 
   bzStrucioStream(char * fn, const char * mode = "rb") { fh = BZ2_bzopen(fn, mode); } 
   int close() { BZ2_bzclose(fh); return 0; }
   size_t Read(void * ptr, size_t bytes) {
	return BZ2_bzread(fh, ptr, bytes);
   }
private:
   BZFILE * fh;
};

/// Input stream using GNU libzlib compression
class gzStrucioStream : public StrucioStream {
public:
   gzStrucioStream(int fd, const char * mode = "rb") { fh = gzdopen(fd, mode); } 
   gzStrucioStream(char * fn, const char * mode = "rb") { fh = gzopen(fn, mode); } 
   int close() { return gzclose(fh); }
   size_t Read(void * ptr, size_t bytes) {
	return gzread(fh, ptr, bytes);
   }
private:
   gzFile fh;
};

inline StrucioStream * StrucioStream::Open(DtsObject fo) {
  //assert(fo->gettype() = fo->dts->requiretype("string"));
  char * filename = (char *)fo->getdata();
  return Open(filename);
}

inline StrucioStream * StrucioStream::Open(const char * filename) {
  int fd;

  if ((!filename) || (!strcmp(filename, "-"))) {
	fd = 0;
  } else {
	fd = open(filename, O_RDONLY, 0);
  }
  if (fd == -1) {
  	fprintf(stderr, "Error opening file %s\n", filename);
	perror("structured open");
	return NULL;
  }

  if (-1 == lseek(fd, 0, SEEK_SET)) {
	// Can't rewind, so just use zlib
        fprintf(stderr, "(stdin,gzip-capable)\n");
	return(new gzStrucioStream(fd));
  } else {
  	char header[3];
	int size = read(fd, header, 3);
  	assert(0 == lseek(fd, 0, SEEK_SET));
  	if (size == 3) {
		if (!memcmp(header, "BZh", 3)) {
			// Bzip
  			fprintf(stderr, "Opening file %s (bzip)\n", filename);
			return(new bzStrucioStream(fd));
		} else if (!memcmp(header, "\037\213", 2) ||  // Gzip
			!memcmp(header, "\037\235", 2)) {     // Compress
  			fprintf(stderr, "Opening file %s (gzip)\n", filename);
			return(new gzStrucioStream(fd));
		}
	}

	// Default is Plain
  	fprintf(stderr, "Opening file %s (plain)\n", filename);
	return(new fStrucioStream(fd));
  }
}

inline void StrucioStream::FileModule(SmacqModule::smacq_init * context) {
  assert(context->argc > 0);
  char ** argv = (char**)malloc(sizeof(char*) * context->argc);
  argv[0] = "file";

  // Duplicate remaining arguments
  for (int i = 1; i < context->argc; i++) {
	argv[i] = context->argv[i];
  }
  //memcpy(argv+1, context->argv+1, context->argc-1 * sizeof(char*));
 
  SmacqGraph * g = new SmacqGraph(context->argc, argv);
  context->self->insert(g);
}

