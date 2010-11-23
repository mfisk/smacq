#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SMACQ_MODULE_IS_STATELESS 1
#include <SmacqModule.h>

struct mask {
	struct in_addr mask;
	struct in_addr addr;
	int isnot;
	/* struct mask * next; */
};

SMACQ_MODULE(mask,
  PROTO_CTOR(mask);
  PROTO_CONSUME();

  DtsField field;
  struct mask test;
  dts_typeid ip_type;
);

smacq_result maskModule::consume(DtsObject datum, int & outchan) {
  in_addr_t f;
  int found = 0;

  assert(datum);

  DtsObject fieldo = datum->getfield(field);
  //if (!fieldo) fprintf(stderr, "mask: No such field (%d) on %p\n", field[0], datum);
  if (!fieldo) return SMACQ_FREE;

  if (fieldo->gettype() != ip_type) {
	fprintf(stderr, "field %s is type %s(%d) instead of %s(%d)\n", dts->field_getname(field).c_str(), fieldo->gettypename(), fieldo->gettype(), dts->typename_bynum(ip_type), ip_type);
	return SMACQ_FREE;
  }

  f = dts_data_as(fieldo, in_addr_t);
  	
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

maskModule::maskModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  int i;
  test.isnot = 0;
  int argc = context->argc-1;
  const char ** argv = context->argv+1;

  field = usesfield(argv[0]);
  //fprintf(stderr, "Mask on field %s, %d (env %p)\n", argv[0], field[0], env);

  ip_type = dts->requiretype("ip");

  assert(argc==2);
  for (i = 1; i < argc; i++) {
	  const char * notstr = index(argv[i], '!');

	  if (notstr && (notstr == argv[i])) {
		  test.isnot = 1;
		  argv[i]++;
	  }

	  char * slash = (char *)index(argv[i], '/');
	  int cidr = 0;

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
