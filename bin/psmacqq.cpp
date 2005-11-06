#include <smacq_am.h>

int main(int argc, char ** argv) {
  init_am(&argc, &argv);

  if (gasnet_mynode() == 0) {
	smacqq(argc, argv);
  	gasnet_exit(0);
  } else {
	smacq_am_slave_loop();
	return 0;
  }
}

