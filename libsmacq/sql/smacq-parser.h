struct graph {
      struct filter * head;
      struct filter * tail;
};

struct vphrase {
      char * verb;
      struct arglist * args;
};

struct group {
	struct arglist * args;
	struct arglist * having;
};


