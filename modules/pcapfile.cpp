#include <stdio.h>
#include <SmacqModule.h>

SMACQ_MODULE(pcapfile, 
  PROTO_CTOR(pcapfile);
);

pcapfileModule::pcapfileModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  fprintf(stderr, "Error:  The pcapfile module has been replaced with pcapread and pcapwrite.\n");
  exit(-1);
}

