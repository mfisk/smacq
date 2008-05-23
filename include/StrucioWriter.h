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

	bool write(void * record, size_t len);

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
	time_t file_end_time;

	Filelist * filelist;
	void newFilelist(Filelist *);

	int openwrite();
	void close_file();

        void format_filename(char * buf, size_t len) {
	   int baselen = strlen(buf);
	   char * basename = (char*)alloca(baselen + 1);
	   assert(basename);
	   strcpy(basename, buf);
	
	   char * p;
           while ((p = strstr(buf, "%T"))) {
		const char * baseoff = basename + baselen - (strlen(buf) - (p+2 - buf)) ;
                char now[32];
                time_t t = time(NULL);
                nstrftime(now, 31, "%Y-%m-%dT%H:%M:%S%:z", localtime(&t), 0, 0);

                p[0] = '\0';
                g_strlcat(buf, now, len);
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
}

inline void StrucioWriter::close_file() {
  if (fs) delete fs;
  fs = NULL;
}

/* -1 iff error */
/* 0 on success */
inline int StrucioWriter::openwrite() {
  char filename[256];
  
  if (!fs) {
    if (maxfilesize) {
      outputleft = maxfilesize;
    }
    if (maxfileseconds) {
      file_end_time = ((time(NULL) / maxfileseconds) + 1) * maxfileseconds;
    }

    filelist->nextfilename(filename, 256);
    format_filename(filename, 256);
    fprintf(stderr, "Output will be placed in structured file %s\n", filename);
    
    if (use_gzip) {
      	fs = new FileStream<gzFile>(filename, "wb");
    } else {
      	fs = new FileStream<FILE*>(filename, "wb");
    }
    assert(fs);

    newfile_hook();
  }

  return 0;
}

inline bool StrucioWriter::write(void * record, size_t len) {
  assert(record);

  if (maxfileseconds && (time(NULL) >= file_end_time)) {
    close_file();
  }

  outputleft -= len;
  //fprintf(stderr, "%lld left\n", outputleft);

  if (maxfilesize && (outputleft <= 0)) {
    close_file();
  }

  if (0 != StrucioWriter::openwrite()) 
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
  filename(NULL), fs(NULL), outputleft(0), maxfilesize(0), maxfileseconds(0),
  filelist(NULL)
{ }

inline StrucioWriter::~StrucioWriter() {
  close_file();
  if (filelist) {
    delete filelist;
    filelist = NULL;
  }
}

