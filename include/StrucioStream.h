#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <zlib.h>
#include <bzlib.h>
#include <DtsObject.h>
#include <SmacqModule.h>

/// Pure-virtual base class for input streams.
class StrucioStream {
 public:
  /// Construct by name.
  StrucioStream(const char * fname, const char * fmode = "rb") 
    : follow(false), filename(fname) 
    {
      // want to do the following, but can't:
      //this->Open(fname, fmode);
    }
    
    /// Construct from open file descriptor
    StrucioStream(const char * fname, const int fileno, const char * fmode = "rb") 
      : follow(false), filename(fname) 
      {
	// want to do the following, but can't:
	//	this->Open(fileno, fmode);
      }
      
      /// Tell this stream to follow file changes
      void Follow();
      
      /// Read from stream  
      size_t Read(void * ptr, size_t bytes);
      
      DtsObject construct(DTS * dts, dts_typeid t) { 
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
      DtsObject construct(DTS * dts, dts_typeid t, unsigned int size) { 
	DtsObject o = dts->newObject(t, size);
	assert(size == this->Read(o->getdata(), size));
	return o;
      }
      
  /// Return the appropriate subclass based on file type 
  static StrucioStream * MagicOpen(const char * filename, const char * mode = "rb");

  /// Open a file specified in a DtsObject and return a StrucioStream object for it.
  static StrucioStream * MagicOpen(DtsObject fo);

 protected:
      bool follow;
      const char * filename;
      int fd;
      ino_t inode;
      const char * mode;
      
      /// Open stream by name.
      virtual bool Open(const char * filename, const char * mode = "rb") = 0;
      
      /// Open stream by file descriptor.
      virtual bool Open(const int fd, const char * mode = "rb") = 0;
      
      /// Read from stream.
      virtual size_t BasicRead(void * ptr, size_t bytes) = 0;
      
      /// Close stream.
      virtual bool Close() = 0;
};


template <typename filetype>
class FileStream : public StrucioStream {
 public:
   FileStream(const char * fname, const int fileno, const char * fmode) :
     StrucioStream(fname, fileno, fmode) 
     {
       Open(fileno, fmode);
     }
     
     FileStream(const char * fname, const char * fmode) :
       StrucioStream(fname, fmode) 
       {
	 Open(fname, fmode);
       }

       bool Open(int fd, const char * mode = "rb");
       bool Open(const char * fn, const char * mode = "rb") ;
       bool Close();
       // XXX. this class could use mmap to implement construct() methods.
       size_t BasicRead(void * ptr, size_t bytes);
       
 private:
       filetype fh;
};


class bzFile_t {
  public:
   bzFile_t(BZFILE* nfh) : fh(nfh) {};
   BZFILE * fh;
};

template<> inline bool FileStream<FILE*>::Open(int fd, const char *mode) {
  fh = fdopen(fd, mode);
  return fh;
}
template<> inline bool FileStream<gzFile>::Open(int fd, const char *mode) {
  fh = gzdopen(fd, mode);
  return fh;
}
template<> inline bool FileStream<bzFile_t>::Open(int fd, const char *mode) {
  fh = BZ2_bzdopen(fd, mode);
  return fh.fh;
}

template<> inline bool FileStream<FILE*>::Open(const char * fn, const char *mode) {
  fh = fopen(fn, mode);
  return fh;
}
template<> inline bool FileStream<gzFile>::Open(const char * fn, const char *mode) {
  fh = gzopen(fn, mode);
  return fh;
}
template<> inline bool FileStream<bzFile_t>::Open(const char * fn, const char *mode) {
  fh = BZ2_bzopen(fn, mode);
  return fh.fh;
}

template<> inline bool FileStream<FILE*>::Close() {
  return 0==fclose(fh);
}
template<> inline bool FileStream<gzFile>::Close() {
  return 0==gzclose(fh);
}
template<> inline bool FileStream<bzFile_t>::Close() {
  BZ2_bzclose(fh.fh);
  return 0;
}

template<> inline size_t FileStream<FILE*>::BasicRead(void * ptr, size_t bytes) {
  return fread(ptr, 1, bytes, fh);
}
template<> inline size_t FileStream<gzFile>::BasicRead(void * ptr, size_t bytes) {
  return gzread(fh, ptr, bytes);
}
template<> inline size_t FileStream<bzFile_t>::BasicRead(void * ptr, size_t bytes) {
  return BZ2_bzread(fh.fh, ptr, bytes);
}

inline StrucioStream * StrucioStream::MagicOpen(const char * fname, const char * mode) {
  int fd;

  if ((!fname) || (!strcmp(fname, "-"))) {
	fd = 0;
  } else {
	fd = open(fname, O_RDONLY, 0);
  }
  if (fd == -1) {
  	fprintf(stderr, "Error opening file %s\n", fname);
	perror("structured open");
	return NULL;
  }

  if (-1 == lseek(fd, 0, SEEK_SET)) {
	// Can't rewind, so just use zlib
        fprintf(stderr, "(stdin,gzip-capable)\n");
	return(new FileStream<gzFile>(fname, fd, mode));
  } else {
  	char header[3];
	int size = read(fd, header, 3);
  	assert(0 == lseek(fd, 0, SEEK_SET));
  	if (size == 3) {
		if (!memcmp(header, "BZh", 3)) {
			// Bzip
  			fprintf(stderr, "Opening file %s (bzip)\n", fname);
			return(new FileStream<BZFILE*>(fname, fd, mode));
		} else if (!memcmp(header, "\037\213", 2) ||  // Gzip
			!memcmp(header, "\037\235", 2)) {     // Compress
  			fprintf(stderr, "Opening file %s (gzip)\n", fname);
			return(new FileStream<gzFile>(fname, fd, mode));
		}
	}

	// Default is Plain
  	fprintf(stderr, "Opening file %s (plain)\n", fname);
	return(new FileStream<FILE*>(fname, fd, mode));
  }
}

inline size_t StrucioStream::Read(void * ptr, size_t bytes) {
  size_t bytes_left = bytes;
  unsigned char * cur_ptr = (unsigned char *)ptr;
  while (1) {
     int res = BasicRead(cur_ptr, bytes_left);
     if (res == (int)bytes_left) return bytes;
     if (!this->follow) {
	return res; 
     } else {
	struct stat st;
        if (0==stat(filename, &st) && (st.st_ino != inode)) {
	  // File has been replaced, use new one.
	  Close();
	  this->Open(filename, mode);
	  Follow(); // Update inode #
	  
	  // We do NOT use any remaining bytes in this file.
	  // This means that files must be split on record boundaries,
	  // but that one corrupt file won't mess up the stream for eternity.
	  // The later problem has proven to be more important.
	} else {
	  // Wait for more input on this file
	  if (res > 0) {
	    bytes_left -= res;
	    cur_ptr += res;
	  }
	  
	  // XXX. Should we return on errors rather than just waiting?
	  ;
	  
	  sleep(1);
	  continue;
	}
     } 
  }
}

inline void StrucioStream::Follow() {
    this->follow = true;
    
    // Stat the file and cache the inode.
    struct stat st;
    if (0 == fstat(fd, &st)) {
	this->inode = st.st_ino;
    } else {
	perror("fstat");
    }
}

inline StrucioStream * StrucioStream::MagicOpen(DtsObject fo) {
  //assert(fo->gettype() = fo->dts->requiretype("string"));
  char * filename = (char *)fo->getdata();
  StrucioStream * o = MagicOpen(filename);
  DtsObject doFollow = fo->getfield("doFollow", false);
  if (doFollow) {
	o->Follow();
  }
  return o;
}

/// Called by modules that wish to delegate file opening to fileModule.
inline void SmacqFileModule(SmacqModule::smacq_init * context) {
  assert(context->argc > 0);
  char ** argv = (char**)malloc(sizeof(char*) * context->argc);
  argv[0] = "file";

  // Duplicate remaining arguments
  for (int i = 1; i < context->argc; i++) {
	argv[i] = context->argv[i];
  }

  //memcpy(argv+1, context->argv+1, context->argc-1 * sizeof(char*));
 
  SmacqGraph * g = new SmacqGraph(context->argc, argv);
  context->self->dynamic_insert(g, context->dts, context->scheduler);
}

