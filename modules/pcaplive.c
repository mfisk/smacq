#undef WRAP_PCAP
#ifdef WRAP_PCAP
#define pcap_loop __real_pcap_loop
#define pcap_datalink __real_pcap_datalink
#define pcap_open __real_pcap_open
#define pcap_perror __real_pcap_perror
#define pcap_close __real_pcap_close
#define pcap_dump_close __real_pcap_dump_close
#define pcap_compile __real_pcap_compile
#define pcap_lookupnet __real_pcap_lookupnet
#define pcap_setfilter __real_pcap_setfilter
#define pcap_open_live __real_pcap_open_live
#define pcap_snapshot __real_pcap_snapshot
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <net/ethernet.h>
#include <smacq.h>
#include "pcapfile.h"

static struct smacq_options options[] = {
  {"i", {string_t:"any"}, "Interface", SMACQ_OPT_TYPE_STRING},
  {"p", {boolean_t:0}, "Promiscuous", SMACQ_OPT_TYPE_BOOLEAN},
  {"s", {int_t:68}, "Snaplen", SMACQ_OPT_TYPE_INT},
  {NULL, {string_t:NULL}, NULL, 0}
};

static void ProcessPacket(struct state * state, struct old_pcap_pkthdr * hdr, 
		   struct ether_header * ethhdr) {
  struct dts_pkthdr * pkt;

  state->datum = (dts_object*)flow_alloc(state->env, hdr->len + sizeof(struct dts_pkthdr), 0);
  flow_datum_settype(state->datum, state->dts_pkthdr_type);
  pkt = (struct dts_pkthdr*)dts_getdata(state->datum);

  pkt->pcap_pkthdr = *hdr;
  // extended header is uninitialized
  pkt->linktype = pcap_datalink(state->pcap);
  pkt->snaplen = pcap_snapshot(state->pcap);

  memcpy(dts_getdata(state->datum) + sizeof(struct dts_pkthdr), ethhdr, hdr->caplen);

}

static smacq_result pcaplive_produce(struct state* state, const dts_object ** datump, int * outchan) {
  int retval;

  if (!state->produce) return SMACQ_ERROR;

  assert(state->pcap);
  state->datum = NULL;

  retval = pcap_loop(state->pcap, 1, (pcap_handler)ProcessPacket, (u_char*)state);
  assert(!retval);

  *datump = state->datum;

  return(SMACQ_PASS|SMACQ_PRODUCE);
}

static smacq_result pcaplive_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);

  assert(0);

  if (state->dts_pkthdr_type == dts_gettype(datum)) {
    //struct dts_pkthdr * pkt = datum->data;
	//xxx_inject;

  } else {
	fprintf(stderr, "Received unknown structure type\n");
	exit(-1);
  }
	
  return SMACQ_FREE;
}


static int pcaplive_shutdown(struct state * state) {
	if (state->dumper) {
		pcap_dump_close(state->dumper);
		state->dumper = NULL;
	}
	if (state->pcap) {
		pcap_close(state->pcap);
		state->pcap = NULL;
	}
	return 0;
}

static char * merge_args(int argc, char ** argv) {
  int len = argc;
  int i;
  char * buf, *bp;
  
  for (i=0; i<argc; i++) 
    len += strlen(argv[i]);
  
  bp = buf = g_malloc(len);
  
  for (i=0; i<argc; i++) {
    strcpy(bp, argv[i]);
    bp += strlen(argv[i]);
    bp[0] = ' ';
    bp++;
  }

  //fprintf(stderr, "Filter is %s (%d)\n", buf, len);
  return buf;
}
  

static int pcaplive_init(struct smacq_init * context) {
  char ebuf[PCAP_ERRBUF_SIZE];
  struct state * state;
  smacq_opt snapleno, promisco, interfaceo;

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);
  state->env = context->env;

  {

    struct smacq_optval optvals[] = {
      { "i", &interfaceo}, 
      { "p", &promisco}, 
      { "s", &snapleno}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
  }

  if (context->isfirst) {
	if (context->islast) {
		fprintf(stderr, "Error: nobody to produce for!\n");
		exit(-1);
	}
	state->pcap = pcap_open_live(interfaceo.string_t, snapleno.int_t, promisco.boolean_t, 0, ebuf);
	if (! state->pcap) {
	    fprintf(stderr, "pcaplive: error: %s\n", ebuf);
	    exit(-1);
	}

	{
	  struct bpf_program filter;
	  bpf_u_int32 net, netmask;
	  char * filterstr = merge_args(state->argc, state->argv);

	  if (pcap_lookupnet(interfaceo.string_t, &net, &netmask, ebuf)) 
	    pcap_perror(state->pcap, "pcap_lookupnet");

	  if (pcap_compile(state->pcap, &filter, filterstr, 1, netmask))
	    pcap_perror(state->pcap, "pcap_compile");

	  if (pcap_setfilter(state->pcap, &filter))
	    pcap_perror(state->pcap, "pcap_setfilter");
	  
	  //free(filterstr);
	}

	state->produce = 1;
  } else if (context->islast) {
	fprintf(stderr, "Writing pcapfile to - (Open for consumption only!)\n");
  } else {
	fprintf(stderr, "pcapfile module must be at beginning or end of dataflow\n");
	exit(-1);
  }

  flow_requiretype(context->env, "packet");
  state->dts_pkthdr_type = smacq_opt_typenum_byname(state->env, "packet");

  return 0;
}


/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_pcaplive_table = {
  &pcaplive_produce, 
  &pcaplive_consume,
  &pcaplive_init,
  &pcaplive_shutdown
};

