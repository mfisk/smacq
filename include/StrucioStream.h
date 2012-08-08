#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 1
#else
# ifndef O_LARGEFILE
/* MacOS needs no additional flag */
# define O_LARGEFILE 0
# endif
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <zlib.h>
#include <bzlib.h>
#include <DtsObject.h>
#include <SmacqModule-interface.h>
#include <SmacqGraph.h>

/// Pure-virtual base class for input streams.
class StrucioStream {
 public:
  // We're happy with the default destructor, but gcc 4.0 requires a virtual destructor for virtual classes...
  virtual ~StrucioStream() {
      // want to use a virtual, but can't from the base constructor
      //this->Close();
  }

  StrucioStream(const char * fname, const char * fmode = "rb") 
    : follow(false), filename(fname), mode(fmode)
    {
      // want to use a virtual, but can't from the base constructor
      //this->Open();
    }
    
    /// Construct from open file descriptor
  StrucioStream(const char * fname, const int fileno, const char * fmode = "rb") 
    : follow(false), filename(fname), fd(fileno), mode(fmode)
    {
      // want to use a virtual, but can't from the base constructor
      //this->FdOpen();
    }
      
      /// Tell this stream to follow file changes
      void Follow();
      
      /// Read from stream  
      size_t Read(void * ptr, size_t bytes);
 
      /// Read from stream  
      virtual size_t Write(void * ptr, size_t bytes) = 0;
 
      /// Construct a fixed-sized object.  
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

  /// Return the file name specified in a DtsObject.
  static std::string Filename(DtsObject fo);

  int getfd() { return fd; }

 protected:
      bool follow;
      ino_t inode;
      const char * filename;

      /// Read from stream.
      virtual size_t BasicRead(void * ptr, size_t bytes) = 0;
      
      /// Close stream.
      virtual bool Close() = 0;

   /// We always keep a valid fd number around.
   int fd;

   /// Desired open mode (fopen syntax)
   const char * mode;

   virtual bool FdOpen() = 0;

   /// (Re)Open stream by name.
   bool Open() {
	if (!strcmp(mode, "r") || !strcmp(mode, "rb")) {
		if (!strcmp(filename, "-")) {
			fd = 0;
		} else {
			fd = open(filename, O_RDONLY|O_LARGEFILE, 0666);
		}
	} else if (!strcmp(mode, "w") || !strcmp(mode, "wb")) {
		if (!strcmp(filename, "-")) {
			fd = 1;
		} else {
			fd = open(filename, O_WRONLY|O_TRUNC|O_CREAT|O_LARGEFILE, 0666);
		}
	} else {
		fprintf(stderr, "Unsupported Open mode: %s\n", mode);
		return false;
	}
	if (fd == -1) {
		perror("open");
		return false;
	}
	return FdOpen();
   }

      
};


template <typename filetype>
class FileStream : public StrucioStream {
 public:
   FileStream(const char * fname, const int fileno, const char * fmode = "rb") :
     StrucioStream(fname, fileno, fmode), fh(NULL)
     {
       FdOpen();
     }
   
   FileStream(const char * fname, const char * fmode = "rb") :
     StrucioStream(fname, fmode), fh(NULL)
     {
       Open();
     }
   
   ~FileStream() {
	if (fh) Close();
   }
     
   bool Close();

   // XXX. this class could use mmap to implement construct() methods.
   size_t BasicRead(void * ptr, size_t bytes);
   size_t Write(void * ptr, size_t bytes);

 protected:      

   /// Open stream by file descriptor.
   bool FdOpen();


 private:

   filetype fh;
};


class bzFile_t {
  public:
   bzFile_t(BZFILE* nfh) : fh(nfh) {};
   BZFILE * fh;
};

template<> inline bool FileStream<FILE*>::FdOpen() {
  fh = fdopen(fd, mode);
  return fh;
}
template<> inline bool FileStream<gzFile>::FdOpen() {
  fh = gzdopen(fd, mode);
  return fh;
}
template<> inline bool FileStream<bzFile_t>::FdOpen() {
  fh = BZ2_bzdopen(fd, mode);
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

template<> inline size_t FileStream<FILE*>::Write(void * ptr, size_t bytes) {
  return fwrite(ptr, 1, bytes, fh);
}
template<> inline size_t FileStream<gzFile>::Write(void * ptr, size_t bytes) {
  return gzwrite(fh, ptr, bytes);
}
template<> inline size_t FileStream<bzFile_t>::Write(void * ptr, size_t bytes) {
  return BZ2_bzwrite(fh.fh, ptr, bytes);
}

inline StrucioStream * StrucioStream::MagicOpen(const char * fname, const char * mode) {
  int fd;

  if ((!fname) || (!strcmp(fname, "-"))) {
	fd = 0;
  } else {
	fd = open(fname, O_RDONLY|O_LARGEFILE, 0666);
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
  			//fprintf(stderr, "Opening file %s (bzip)\n", fname);
			return(new FileStream<BZFILE*>(fname, fd, mode));
		} else if (!memcmp(header, "\037\213", 2) ||  // Gzip
			!memcmp(header, "\037\235", 2)) {     // Compress
  			//fprintf(stderr, "Opening file %s (gzip)\n", fname);
			return(new FileStream<gzFile>(fname, fd, mode));
		}
	}

	// Default is Plain
  	//fprintf(stderr, "Opening file %s (plain)\n", fname);
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
	  Open();
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

inline std::string StrucioStream::Filename(DtsObject fo) {
  //assert(fo->gettype() = fo->dts->requiretype("string"));
  std::string filename((char *)fo->getdata(), fo->getsize());
  return filename;
}

inline StrucioStream * StrucioStream::MagicOpen(DtsObject fo) {
  std::string filename = Filename(fo);
  StrucioStream * o = MagicOpen(filename.c_str());
  DtsObject doFollow = fo->getfield("doFollow", true);
  if (doFollow) {
	o->Follow();
  }
  return o;
}

/// Called by modules that wish to delegate file opening to fileModule.
inline void SmacqFileModule(SmacqModule::smacq_init * context) {
  assert(context->argc > 0);
  const char ** argv = (const char**)malloc(sizeof(char*) * context->argc);
  argv[0] = "file";

  // Duplicate remaining arguments
  for (int i = 1; i < context->argc; i++) {
	argv[i] = context->argv[i];
  }

  //memcpy(argv+1, context->argv+1, context->argc-1 * sizeof(char*));
 
  SmacqGraphNode * g = new SmacqGraphNode(context->argc, argv);
  context->self->dynamic_insert(g, context->dts);
}

