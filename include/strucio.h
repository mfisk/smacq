struct strucio;

struct strucio * strucio_init();
int strucio_open(struct strucio * rdr);
void * strucio_read(struct strucio *, void * buf, int len);
void * strucio_read_mmap(struct strucio *, int len);
void * strucio_read_copy(struct strucio *, void * buf, int len);
int strucio_write(struct strucio *, void * record, int len);
void strucio_close(struct strucio *);

enum strucio_read_type { EITHER, COPY, MMAP };
typedef char * strucio_nextfilename_fn(struct strucio *, void *);
typedef int strucio_newfile_fn(struct strucio *, void *);

/* Use one of the following: */
void strucio_register_filelist(struct strucio *, strucio_nextfilename_fn * fn, void *);
void strucio_register_filelist_stdin(struct strucio *);
void strucio_register_filelist_args(struct strucio * r, int argc, char ** argv);
void strucio_register_filelist_bounded(struct strucio * r, char * index_location, long long lower, long long upper);
void strucio_register_file(struct strucio *, char * filename);

void strucio_set_rotate(struct strucio *, long long size);
void strucio_register_newfile(struct strucio *, strucio_newfile_fn *, void *);
void strucio_set_read_type(struct strucio *, enum strucio_read_type read_type);
void strucio_set_use_gzip(struct strucio *, int boolean);

//char * strucio_filelist_stdin(struct strucio * r, void * data);
