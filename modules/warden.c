#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <dts_packet.h>

static struct smacq_options options[] = {
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  smacq_environment * env;
  const dts_object * datum;
}; 

static inline int min(int a, int b) {
  return (a<b ? a : b);
}

static smacq_result warden_consume(struct state * state, const dts_object * datum, int * outchan) {
  struct dts_pkthdr * pkthdr;
  struct ethhdr * ethhdr;
  struct iphdr * iphdr;
  struct tcphdr * tcphdr;

 state->datum = dts_writable(state->env, datum);
 pkthdr = (struct dts_pkthdr*)state->datum->data;
 ethhdr = (struct ethhdr*)(pkthdr+1);
 if (ethhdr->h_proto != htons(ETH_P_IP)) return SMACQ_PASS; // Not IP
 else iphdr = (struct iphdr *)(ethhdr+1);

 if (iphdr->protocol != 6) return SMACQ_PASS;
 else tcphdr = (struct tcphdr *)((char *)iphdr + (iphdr->ihl<<2));
 
 // Urgent ptr without urgent bit
 if (tcphdr->urg) {
   tcphdr->urg_ptr = 0;
 }

 // Reserved bits
 tcphdr->res1 = 0;
 tcphdr->res2 = 0;

 // RST implies no TCP payload
 if (tcphdr->rst) {
   // Make sure IP payload size = TCP header only
   iphdr->tot_len = htons((iphdr->ihl<<2) + (tcphdr->doff << 2));
 }

 // Urgent pointer must be within TCP payload
 if (tcphdr->urg_ptr > (ntohs(iphdr->tot_len) - (tcphdr->doff<<2))) {
   tcphdr->urg_ptr = 0;
   tcphdr->urg = 0;
}

 // Remove covert trailers in the payload
 pkthdr->pcap_pkthdr.caplen = pkthdr->pcap_pkthdr.len = ntohs(iphdr->tot_len) + sizeof(struct ethhdr);

 return SMACQ_FREE|SMACQ_PRODUCE;
}

static int warden_init(struct flow_init * context) {
  int argc = 0;
  char ** argv;
  struct state * state = context->state = g_new0(struct state, 1);
  state->env = context->env;

  {
    struct smacq_optval optvals[] = {
      {NULL, NULL}
    };
    flow_getoptsbyname(context->argc-1, context->argv+1,
		       &argc, &argv,
		       options, optvals);
    
  }


  return 0;
}

static int warden_shutdown(struct state * state) {
  return SMACQ_END;
}


static smacq_result warden_produce(struct state * state, const dts_object ** datump, int * outchan) {
  if (state->datum) {
    *datump = state->datum;
    state->datum = NULL;
    return SMACQ_PASS;
  } else {
    return SMACQ_END;
  }
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_warden_table = {
  &warden_produce, 
  &warden_consume,
  &warden_init,
  &warden_shutdown
};
