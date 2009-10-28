#include <stdio.h>
#include <zlib.h>
#include <alloca.h>
#include <Filelist.h>
#include <StrucioStream.h>

BEGIN_C_DECLS
#include "strftime.h" // gnulib strftime with portable %z
END_C_DECLS

enum strucio_read_type { EITHER, COPY, MMAP };

/// A file writer for structured data.
class StrucioWriter {
   public:
	StrucioWriter();
	virtual ~StrucioWriter();

        /// Write a buffer of specified length.  Update current time to now if specified.
	bool write(void * record, size_t len, time_t now = 0);

	/* Called when new file opened */
	virtual void newfile_hook() { /* Base class does nothin. */ };

	/* Use one of the following: */
	void register_filelist_stdin();
	void register_filelist_args(int argc, char ** argv);
	void register_filelist_bounded(char * index_location, long long lower, long long upper);
	void register_file(const std::string &);
	void register_file(const char *);
	
	void set_rotate_time(long long);
	void set_rotate_size(long long);
	void set_use_gzip(bool val) { use_gzip = val;}

   protected:
  	/* Currenly opened file */
  	char * filename;
  	StrucioStream * fs;

  	/* For zlib */
  	bool use_gzip;

  	/* For rotation */
  	long long outputleft;
  	long long maxfilesize;
  	long long maxfileseconds;
	time_t file_end_time, now;

	Filelist * filelist;
	void newFilelist(Filelist *);

	int openwrite();
	void close_file();

        void format_filename(char * basename, char * buf, size_t len) {
           g_strlcpy(buf, basename, len);
	   char * p;
           while ((p = strstr(buf, "%T"))) {
		const char * baseoff = basename + strlen(basename) - (strlen(buf) - (p+2 - buf)) ;
                char nowstr[32];
                nstrftime(nowstr, 31, "%Y-%m-%dT%H:%M:%S%:z", localtime(&now), 0, 0);

                p[0] = '\0';
                g_strlcat(buf, nowstr, len);
		g_strlcat(buf, baseoff, len);
           }
        }
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

inline void StrucioWriter::register_file(const std::string & filename) {
  filelist = new FilelistConstant(filename);
}

inline void StrucioWriter::register_file(const char * filename) {
  filelist = new FilelistConstant(filename);
}

inline void StrucioWriter::set_rotate_size(long long size) {
  maxfilesize = size;
}

inline void StrucioWriter::set_rotate_time(long long seconds) {
  maxfileseconds = seconds;
  file_end_time = 0;
}

inline void StrucioWriter::close_file() {
  if (fs) delete fs;
  fs = NULL;
}

/// Make sure there is an open filestream for writing.
/// Return -1 iff error; 0 on success 
inline int StrucioWriter::openwrite() {
  char filename[256], thisfilename[256];
  
  if (!fs) {
    if (maxfilesize) {
      outputleft = maxfilesize;
    }
    if (maxfileseconds) {
      file_end_time = ((now / maxfileseconds) + 1) * maxfileseconds;
    }

    filelist->nextfilename(filename, 256);
    format_filename(filename, thisfilename, 256);
    fprintf(stderr, "Output will be placed in structured file %s\n", thisfilename);
    
    if (use_gzip) {
      	fs = new FileStream<gzFile>(thisfilename, "wb");
    } else {
      	fs = new FileStream<FILE*>(thisfilename, "wb");
    }
    assert(fs);

    newfile_hook();
  }

  return 0;
}

inline bool StrucioWriter::write(void * record, size_t len, time_t t) {
  assert(record);
  if (t) {
    now = t;
  }

  if (maxfileseconds) {
    if (!now) {
       now = time(NULL);
    }
    if (now >= file_end_time) {
       close_file();
    }
  }

  outputleft -= len;
  //fprintf(stderr, "%lld left\n", outputleft);

  if (maxfilesize && (outputleft <= 0)) {
    close_file();
  }

  if (0 != openwrite()) 
    return false;
  
  if (len != fs->Write(record, len)) {
    perror("structured write");
    return false;
  }
	
  return true;
}

inline void StrucioWriter::newFilelist(Filelist * fl) {
  if (filelist) delete filelist;
  filelist = fl;
}

inline StrucioWriter::StrucioWriter() :
  filename(NULL), fs(NULL), outputleft(0), maxfilesize(0), maxfileseconds(0), now(0),
  filelist(NULL)
{ }

inline StrucioWriter::~StrucioWriter() {
  close_file();
  if (filelist) {
    delete filelist;
    filelist = NULL;
  }
}

