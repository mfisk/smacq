#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "smacq.h"

struct mask {
	struct in_addr mask;
	struct in_addr addr;
	int isnot;
	/* struct mask * next; */
};

#define SMACQ_MODULE_IS_STATELESS 1

SMACQ_MODULE(mask,
  PROTO_CTOR(mask);
  PROTO_CONSUME();

  dts_field field;
  struct mask test;
  int ip_type;
);

static struct smacq_options options[] = {
  //{"f", {string_t:NULL}, "Field to inspect (full data is default)", SMACQ_OPT_TYPE_STRING},
  //{"m", {boolean_t:0}, "OR multiple fields and demux to individual outputs", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

smacq_result maskModule::consume(DtsObject datum, int * outchan) {
  DtsObject fieldo;
  in_addr_t f;
  int found = 0;

  assert(datum);

  if (field) {
  	fieldo = datum->getfield(field);
  	//if (!field) fprintf(stderr, "mask: No such field (%d) on %p\n", field[0], datum);
  	if (!fieldo) return SMACQ_FREE;
	assert(fieldo->gettype() == ip_type);

        f = dts_data_as(fieldo, in_addr_t);
  	
  } else {
	assert(datum->gettype() == ip_type);
        f = dts_data_as(datum, in_addr_t);
  }

  //fprintf(stderr, "%x & %x =? %x\n", f, test.mask.s_addr, test.addr.s_addr); 
  if ((f & test.mask.s_addr) == test.addr.s_addr) {
	  found = 1;
  }

  if (test.isnot) {
	  return (found ? SMACQ_FREE : SMACQ_PASS);
  } else {
	  return (found ? SMACQ_PASS : SMACQ_FREE);
  }
}

maskModule::maskModule(struct smacq_init * context) : SmacqModule(context) {
  int i;
  int argc = context->argc-1;
  char ** argv = context->argv+1;

  field = dts->requirefield(argv[0]);
  //fprintf(stderr, "Mask on field %s, %d (env %p)\n", argv[0], field[0], env);
  
  ip_type = dts->requiretype("ip");

  assert(argc==2);
  for (i = 1; i < argc; i++) {
	  char * slash = index(argv[i], '/');
	  char * notstr = index(argv[i], '!');

	  int cidr = 0;

	  if (notstr && (notstr == argv[i])) {
		  test.isnot = 1;
		  argv[i]++;
   	  }

	  if (slash) {
		  slash[0] = '\0';
		  cidr = atoi(slash+1);
	  } else {
		  cidr = 32;
	  }

	  test.mask.s_addr = ((~0) << (32 - cidr));
	  test.mask.s_addr = htonl(test.mask.s_addr);

	  if (!inet_aton(argv[i], &test.addr)) {
		fprintf(stderr, "mask: Unable to parse address %s.\n", argv[i]);
		assert(0);
	  }

	  //fprintf(stderr, "masking %s with /%d, %s\n", strdup(inet_ntoa(test.addr)), cidr, strdup(inet_ntoa(test.mask)));
  }
}

