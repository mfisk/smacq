#include <smacq.h>

struct graph {
      smacq_graph * head;
      smacq_graph * tail;
};

struct vphrase {
      char * verb;
      struct arglist * args;
};

struct group {
      struct arglist * args;
      struct arglist * having;
};


