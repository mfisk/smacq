#include <smacq.h>

int epan_getfield(const dts_object * packet, dts_object * fieldo, dts_field_element element) {
	fprintf(stderr, "packet: warning: EPAN fields not available in this build\n");
	return 0;
}
