#include <smacq.h>

static int Warned = 0;

int epan_getfield(DtsObject * packet, DtsObject * fieldo, dts_field_element element) {
	if (!Warned) {
		fprintf(stderr, "packet: warning: EPAN fields not available in this build\n");
		Warned = 1;
	}
	return 0;
}
