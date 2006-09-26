#include <stdio.h>
#include <smacq.h>

int main(int argc, char ** argv) {
  DTS dts;
  SmacqScheduler s(4); // Use 4 CPUs (threads)

  SmacqGraphContainer graphs;
  graphs.addQuery(&dts, &s, argv2string(argc-1, argv+1));
  graphs.init(&dts, &s);

  for (;;) {
  	DtsObject output;

	// element() does a little bit of work.
	// If you are willing to surrender the CPU for as long as 
	// it takes to get some output, then use get() instead.
	smacq_result r = s.element(output);

	if (r & SMACQ_PASS) {
		// This means the query returned an object
		fprintf(stderr, "query returned an object\n");

	} else if (r == SMACQ_NONE) {
		// This means we're all done
		break;
	} else {
		// This means we did some work, but it didn't output anything this time.
		;
	}
  }

  fprintf(stderr, "Done\n");
  return 0;
}

