/*
 *                      Los Alamos National Laboratory
 *
 *      Copyright, 2003.  The Regents of the University of California.
 *      This software was produced under a U.S. Government contract
 *      (W-7405-ENG-36) by Los Alamos National Laboratory, which is
 *      operated by the University of California for the U.S. Department
 *      of Energy.  The U.S. Government is licensed to use, reproduce,
 *      and distribute this software.  Permission is granted to the
 *      public to copy and use this software without charge, provided
 *      that this Notice and any statement of authorship are reproduced
 *      on all copies.  Neither the Government nor the University makes
 *      any warranty, express or implied, or assumes any liability or
 *      responsibility for the use of this software.
 *
 *      EPAN/Ethereal Filter plug-in module for SMACQ
 *        Author:   Alex Kent (alex@lanl.gov)
 *        $Date: 2003/05/01 20:21:27 $
 *
 *      $Id: epan_filter.c,v 1.1 2003/05/01 20:21:27 alexkent Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* SMACQ includes */
#include "dts_packet.h"
#include "smacq.h"

/* Ethereal includes */
#include <prefs.h>
#include <register.h>
#include <epan/epan.h>
#include <epan/packet.h>

#define EPAN_MODULE_DIR "/usr/local/ethereal/modules"

static struct smacq_options options[] = {
  {NULL, {NULL}, NULL, 0}
};

struct state {
  DTS *env;
  dfilter_t *rfcode;
  epan_dissect *edt;
  frame_data fdate;
};

smacq_result epanModule::consume(struct state *state, DtsObject *datum, int *outchan) {
  int found=0;
  struct dts_pkthdr *dhdr;
  struct old_pcap_pkthdr *phdr;
  char *packet_data;

  assert(datum);

  dhdr=(struct dts_pkthdr *)datum->getdata();
  packet_data=(char *)(dhdr+1);
  phdr=&dhdr->pcap_pkthdr;

  fdata.next=NULL;
  fdata.prev=NULL;
  fdata.pfd=NULL;
  fdata.data_src=NULL;
  fdata.num=0;
  fdata.file_off=0;
  fdata.abs_sec=0;
  fdata.abs_usec=0;
  fdata.flags.passed_dfilter=0;
  fdata.flags.encoding=CHAR_ASCII;
  fdata.flags.visited=0;
  fdata.flags.marked=0;
  fdata.rel_secs=0;
  fdata.rel_usecs=0;
  fdata.del_secs=0;
  fdata.del_usecs=0;

  fdata.pkt_len=phdr->len;
  fdata.cap_len=phdr->caplen;
  fdata.lnk_t=dhdr->linktype;

  /* Setup and run */
  edt=epan_dissect_new(1,0);
  epan_dissect_prime_dfilter(edt,rfcode);
  epan_dissect_run(edt,NULL,packet_data,&fdata,NULL);
  found=dfilter_apply_edt(rfcode,edt);

  /* Cleanup */
  epan_dissect_free(edt);
  if (fdata.pfd) {
    g_slist_free(fdata.pfd);
  }
  free_data_sources(&fdata);

  if (found) {
    return SMACQ_PASS;
  } else {
    return SMACQ_FREE;
  }
}

static int epan_init(struct smacq_init *context) {
  struct state *state;
  int res;
  int argc;
  char **argv;
  struct smacq_optval optvals[] = {
    { NULL, NULL } } ;

  context->state=state=(struct state *)calloc(sizeof(struct state),1);
  assert(state);

  env=context->env;
  
  smacq_getoptsbyname(context->argc-1,
		      context->argv+1,
		      &argc, &argv,
		      options, optvals);
  assert(argc==1);

  epan_init(EPAN_MODULE_DIR,register_all_protocols,register_all_protocol_handoffs);
  prefs_register_modules();
  init_dissection();

  res=dfilter_compile(argv[0],&rfcode);
  if (!res) {
    fprintf(stderr,"EPAN compile error: %s\n",dfilter_error_msg);
  }
  assert(res);
  
  return SMACQ_PASS;
}

static int epan_shutdown(struct state *state) {
  dfilter_free(rfcode);
  return SMACQ_END;
}

struct smacq_functions smacq_epan_table = {
  produce: NULL,
  consume: &epan_consume,
  init: &epan_init,
  shutdown: &epan_shutdown,
} ;
