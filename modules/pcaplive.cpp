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
#include <sys/types.h>
#include <net/ethernet.h>
#include <SmacqModule.h>
#include <pcap.h>
#include <dts_packet.h>
 
static struct smacq_options options[] = {
  {"i", {string_t:"any"}, "Interface", SMACQ_OPT_TYPE_STRING},
  {"p", {boolean_t:0}, "Promiscuous", SMACQ_OPT_TYPE_BOOLEAN},
  {"s", {int_t:68}, "Snaplen", SMACQ_OPT_TYPE_INT},
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(pcaplive, 
  PROTO_CTOR(pcaplive);
  PROTO_DTOR(pcaplive);
  PROTO_PRODUCE();

 public:
  void ProcessPacket(const struct old_pcap_pkthdr * hdr, const struct ether_header * ethhdr);

 private:

	/* State if using libpcap */
  pcap_t * pcap;		
  DtsObject datum;	
  pcap_dumper_t * dumper;

	/* Dynamic dataflow environment */
  int argc;
  char ** argv;
  int dts_pkthdr_type;		

  DtsObject linktype_o;
  DtsObject snaplen_o;

  DtsField linktype_field;
  DtsField snaplen_field;
);

void process_wrapper(u_char * user, const struct pcap_pkthdr * hdr, const u_char * data) {
	struct ether_header * ethhdr = (struct ether_header*)data;
	pcapliveModule * o = (pcapliveModule*)user;
	o->ProcessPacket((const struct old_pcap_pkthdr*)hdr, ethhdr);
}

inline void pcapliveModule::ProcessPacket(const struct old_pcap_pkthdr * hdr, 
		   const struct ether_header * ethhdr) {
  struct dts_pkthdr * pkt;

  datum = dts->newObject(dts_pkthdr_type, hdr->len + sizeof(struct dts_pkthdr));
  pkt = (struct dts_pkthdr*)datum->getdata();

  pkt->pcap_pkthdr = *hdr;
  // extended header is uninitialized

  
  datum->attach_field(snaplen_field, snaplen_o);

  
  datum->attach_field(linktype_field, linktype_o);

  memcpy(datum->getdata() + sizeof(struct dts_pkthdr), ethhdr, hdr->caplen);

}

smacq_result pcapliveModule::produce(DtsObject & datump, int & outchan) {
  int retval;

  assert(pcap);

  retval = pcap_loop(pcap, 1, process_wrapper, (u_char*)this);
  assert(!retval);

  datump = datum;
  return( (smacq_result)(SMACQ_PASS|SMACQ_PRODUCE));
}

pcapliveModule::~pcapliveModule() {
	if (dumper) {
		pcap_dump_close(dumper);
		dumper = NULL;
	}
	if (pcap) {
		pcap_close(pcap);
		pcap = NULL;
	}
}

static char * merge_args(int argc, char ** argv) {
  int len = argc;
  int i;
  char * buf, *bp;
  
  for (i=0; i<argc; i++) 
    len += strlen(argv[i]);
  
  bp = buf = (char*)g_malloc(len);
  
  for (i=0; i<argc; i++) {
    strcpy(bp, argv[i]);
    bp += strlen(argv[i]);
    bp[0] = ' ';
    bp++;
  }

  //fprintf(stderr, "Filter is %s (%d)\n", buf, len);
  return buf;
}
  

pcapliveModule::pcapliveModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  char ebuf[PCAP_ERRBUF_SIZE];
  smacq_opt snapleno, promisco, interfaceo;

  {
    struct smacq_optval optvals[] = {
      { "i", &interfaceo}, 
      { "p", &promisco}, 
      { "s", &snapleno}, 
      {NULL, NULL}
    };
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
  }

  pcap = pcap_open_live(interfaceo.string_t, snapleno.int_t, promisco.boolean_t, 0, ebuf);
  if (! pcap) {
    fprintf(stderr, "pcaplive: error: %s\n", ebuf);
    exit(-1);
  }
  
  {
    struct bpf_program filter;
    bpf_u_int32 net, netmask;
    char * filterstr = merge_args(argc, argv);
    
    if (pcap_lookupnet(interfaceo.string_t, &net, &netmask, ebuf)) 
      pcap_perror(pcap, "pcap_lookupnet");
    
    if (pcap_compile(pcap, &filter, filterstr, 1, netmask))
      pcap_perror(pcap, "pcap_compile");
    
    if (pcap_setfilter(pcap, &filter))
      pcap_perror(pcap, "pcap_setfilter");
    
    //free(filterstr);
  }

  dts->requiretype("packet");
  dts_pkthdr_type = dts->requiretype("packet");

  snaplen_field = dts->requirefield("snaplen");
  linktype_field = dts->requirefield("linktype");


  {
    int linktype = pcap_datalink(pcap);
    int snaplen = pcap_snapshot(pcap);

    int snaplen_type = dts->requiretype("int");
    int linktype_type = dts->requiretype("int");


    snaplen_o = 
      dts->construct(snaplen_type, &snaplen);

    linktype_o = 
      dts->construct(linktype_type, &linktype);
  }
}

