#include <smacq.h>

struct pickle;


struct pickle * pickle_init(void);
void pickle_close_source(struct pickle * pickle, int fd);

int read_datum(smacq_environment * env, struct pickle * pickle, int fd, const dts_object ** datum);
int write_datum(smacq_environment * env, struct pickle * pickle, int fd, const dts_object * datum);


