#include <smacq.h>

struct pickle;


struct pickle * pickle_init(void);
void pickle_close_source(struct pickle * pickle, int fd);

int read_datum(DTS * env, struct pickle * pickle, int fd, DtsObject datum);
int write_datum(DTS * env, struct pickle * pickle, int fd, DtsObject datum);


