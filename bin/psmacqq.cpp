#include <smacq_am.h>

int main(int argc, char ** argv) {
  int retval;
  retval = gasnet_init(&argc, &argv);
  assert (retval == GASNET_OK);
  retval = gasnet_attach(handlers, numHandlers, 0, 0);
  assert (retval == GASNET_OK);

  if (gasnet_mynode() == 0) {
	smacqq(argc, argv);
  	gasnet_exit(0);
  } else {
	bool done = false;
	for (;;) {
		GASNET_BLOCKUNTIL(done);
	}
  }
}

