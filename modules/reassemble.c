#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <smacq.h>
#include <smacq-internal.h>
#include <dts_packet.h>
#include <fields.h>
#include "bytehash.h"

/* Glib and Snort both define MIN and MAX macros.  
 * Undef the glib one (provided by smacq.h) so that we get the Snort one 
 * with the following include 
 */
#undef MIN
#undef MAX

#include <spp_export_pkt.h>

/* Programming constants */

struct state {

  const dts_object * datum;

        /* Dynamic dataflow environment */
  smacq_environment * env;
  int dts_pkthdr_type;

  int produce; /* Does this instance produce */
  int extended;
  struct pcap_file_header pcap_file_header;

  struct fieldset fieldset;
  guint32 memcap;
  unsigned int timeout;

  list_item * list_ptr;
  Packet * newpkt;

  int consume_count;
  int produce_count;
};

static struct smacq_options options[] = {
  {"m", {double_t:0}, "Max amount of memory (bytes)", SMACQ_OPT_TYPE_DOUBLE},
  {"t", {double_t:0}, "Timeout value (secs)", SMACQ_OPT_TYPE_TIMEVAL},
  {NULL, {string_t:NULL}, NULL, 0}
};

void Stream4Init(u_char *);
void Stream4InitReassembler(u_char *);
void PrintCharData(FILE * , char *, int );

list_item * head = NULL, * tail = NULL;
int num_list_items = 0;

/////////////////////////
void list_enqueue(list_item * new) {
/////////////////////////
#ifdef DEBUG
  if (!new) {
    fprintf(stderr, "*** reassemble.c::list_enqueue(): NULL new passed in!\n");
	return;
  }
#endif
  if (num_list_items == 0) {
    head = tail = new; 	
  }
  else {
    tail->next = new;
    tail = new;
  }
  tail->next = NULL;
  num_list_items++;

  #ifdef DEBUG
    fprintf(stderr, "*** reassemble.c::list_enqueue(): item added, Id = %d, list size now = %d\n", ntohs(new->packet->iph->ip_id), num_list_items);
  #endif
}

////////////////////////
list_item * list_dequeue() {
////////////////////////
  list_item * curr;
  if (head) {
    curr = head;
    head = head->next;
    num_list_items--;
  #ifdef DEBUG
    fprintf(stderr, "*** reassemble.c::list_dequeue(): item removed, Id = %d, list size now = %d\n", ntohs(curr->packet->iph->ip_id), num_list_items);
  #endif
    return(curr);
  }
  else {
    #ifdef DEBUG
      fprintf(stderr, "*** reassemble.c::list_dequeue(): NULL LIST HEAD.\n");
    #endif
    return (list_item *)NULL;
  }
}

//////////////
int reassemble_init(struct smacq_init * context) {
//////////////
  int argc;
  char ** argv;

  struct state * state = context->state = g_new0(struct state, 1);
  //u_char * snort_args = "both, ports all";

  assert(state);

#ifdef DEBUG
  fprintf(stderr, "*** reassemble.c::reassemble_init(): entered...\n");
#endif

  state->consume_count = 0;
  state->produce_count = 0;

  signal(SIGQUIT, CleanExit);

  flow_requiretype(context->env, "packet");
  state->env = context->env;

  state->produce = 1;
  {
	smacq_opt memcap, timeout;

  	struct smacq_optval optvals[] = {
      { "m", &memcap },
      { "t", &timeout}, 
      {NULL, NULL}
  	};

  	smacq_getoptsbyname(context->argc-1, context->argv+1,
			       &argc, &argv,
			       options, optvals);

	state->memcap = memcap.double_t;
	state->timeout = timeout.uint_t;
  }

  // Consume rest of arguments as fieldnames
  fields_init(state->env, &state->fieldset, argc, argv);

  if (!state->memcap) {
    state->memcap = 67108864; // 64 Meg
  }
  if (!state->timeout) {
    state->timeout = 6000; // 6000 seconds
  }

#ifdef DEBUG
  fprintf(stderr, "*** reassemble.c::reassemble_init(): memcap  = %d\n", state->memcap);
  fprintf(stderr, "*** reassemble.c::reassemble_init(): timeout = %d\n", state->timeout);
#endif

  state->dts_pkthdr_type = smacq_opt_typenum_byname(context->env, "packet");
  assert(state->dts_pkthdr_type);

#ifdef DEBUG
  fprintf(stderr, "\n");
#endif

  //pv.data_flag = 1; // -d
  //pv.char_data_flag = 1; // -C
  //pv.show2hdr_flag = 1; // -e
  //pv.verbose_bytedump_flag = 1; // -X
  //pv.verbose_flag = 1; // -v

  pv.quiet_flag = 1;
  pv.use_rules = 1;
  pv.readmode_flag = 1; // -r (for DropStats(), called in reassemble_shutdown() )
  pv.checksums_mode = DO_IP_CHECKSUMS | DO_TCP_CHECKSUMS |
					  DO_UDP_CHECKSUMS | DO_ICMP_CHECKSUMS;

  //pv.assurance_mode = ASSURE_ALL; // -z all
  //pv.assurance_mode = ASSURE_EST; // -z est

  InitNetmasks();
  InitProtoNames();

  // TBD - use state->timeout, state->memcap to init preprocessors
  //
  Frag2Init("timeout 6000, memcap 67108864");

  Stream4Init("timeout 6000, memcap 67108864");

  Stream4InitReassembler("both, ports all");

  tail = head = state->list_ptr;

  ExportPktInit((u_char *)(&(state->list_ptr)));

  // TBD - call proper decoder based on link type (datalinks[n])
  //
  datalinks[0] = DLT_EN10MB; // ethernet
  grinders[0] = DecodeEthPkt;
  SetPktProcessor(0);

#ifdef DEBUG
  fprintf(stderr, "\n");
#endif

  return 0;
}

////////////////////////
smacq_result reassemble_consume(struct state * state, const dts_object * datum, int * channel) {
////////////////////////
  struct dts_pkthdr * pkt = datum->data;
  u_char * pkt_data = (u_char *)pkt + sizeof(struct dts_pkthdr);

#ifdef DEBUG
  fprintf(stderr, "*** reassemble.c::reassemble_consume(): entered...\n");
#endif

  state->consume_count++;

  ProcessPacket(NULL, (struct pcap_pkthdr *)&(pkt->pcap_pkthdr), pkt_data);

#ifdef DEBUG
  fprintf(stderr, "*** reassemble.c::reassemble_consume(): leaving...\n");
#endif

  return (SMACQ_FREE | SMACQ_PRODUCE);
}

/////////////////////////
smacq_result reassemble_produce(struct state * state, const dts_object ** datump, int * channel) {
/////////////////////////
  const dts_object * datum = NULL;
  struct dts_pkthdr * pkt;
  Packet *p;
  list_item * li;

#ifdef DEBUG
  fprintf(stderr, "*** reassemble.c::reassemble_produce(): entered...\n");
#endif

  if (!state) { 
    #ifdef DEBUG
      fprintf(stderr, "*** reassemble.c::reassemble_produce(): NULL state!...\n");
    #endif
	return SMACQ_ERROR;
  }

  if (!state->produce) { 
    #ifdef DEBUG
      fprintf(stderr, "*** reassemble.c::reassemble_produce(): state != produce...\n");
    #endif
	return SMACQ_END;
  }

  // Walk the list of queued Packets...
  //
  li = list_dequeue();

  if (li == NULL) {
    return SMACQ_END;
  }
   
    if (!(li->packet)) {
      fprintf(stderr, "*** reassemble.c::reassemble_produce(): NULL li->packet!\n");
      //break;
      return SMACQ_ERROR;
    }

    p = state->newpkt = li->packet;
    state->produce_count++;
    //p = state->newpkt;
    datum = smacq_alloc(
		       state->env, 
		       p->pkth->len + sizeof(struct dts_pkthdr), 
		       state->dts_pkthdr_type
		       );

    pkt = (struct dts_pkthdr *)datum->data;

    #ifdef DEBUG
      fprintf(stderr, "*** reassemble.c::reassemble_produce(): newpkt: p = %p, start of data = %p, pkth = %p, len = %d, ID = %d\n", p, p->pkth + sizeof(struct pcap_pkthdr), p->pkth, p->pkth->len, ntohs(p->iph->ip_id));
    #endif

    memcpy(&(pkt->pcap_pkthdr), (struct pcap_pkthdr *)p->pkth, sizeof(struct pcap_pkthdr));
    memcpy(((void*)pkt) + sizeof(struct dts_pkthdr), p->pkt, ((struct pcap_pkthdr *)p->pkth)->len);

    // now that the copy is complete, release the list_item
    free (li);

    #ifdef DEBUG
    fprintf(stderr, "*** reassemble.c::reassemble_produce(): extended? = %d, refcount = %d, type = %d, datap = %p, linktype = %d, snaplen = %d, ifindex = %d, protocol = %u, pkt_type = %u, caplen = %u, len = %u\n",

	    state->extended,
	    datum->refcount, 
	    datum->type, 
	    pkt, 
	    pkt->linktype, 
	    pkt->snaplen,
	    pkt->extended.ifindex,
	    pkt->extended.protocol,
	    pkt->extended.pkt_type,
	    pkt->pcap_pkthdr.caplen,
	    pkt->pcap_pkthdr.len
	    );
    fprintf(stderr, "\n");
    #endif

    *datump = datum;

#ifdef DEBUG
  fprintf(stderr, "*** reassemble.c::reassemble_produce(): leaving...\n");
#endif

    //return SMACQ_PASS;
    return SMACQ_PASS | SMACQ_PRODUCE;

}

//////////////////
int reassemble_shutdown(struct state * state) {
//////////////////
  list_item * li;

#ifdef DEBUG
  fprintf(stderr, "*** reassemble.c::reassemble_shutdown(): entered...\n");
#endif

  // TDB - handle via config file or cmd-line args
#ifdef TEST
  pv.quiet_flag = 0;
  DropStats(0);
  fprintf(stderr, "*** reassemble.c::reassemble_shutdown(): consume_count = %d, produce_count = %d\n", state->consume_count, state->produce_count);
#endif

  // make sure the Packet list is freed
  while ((li = list_dequeue()) != NULL) {
	free(li); 
  }

  free(state);

  return 0;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_reassemble_table = {
  &reassemble_produce, 
  &reassemble_consume,
  &reassemble_init,
  &reassemble_shutdown
};

