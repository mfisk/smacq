#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include <SmacqModule.h>
#include <dts_packet.h>
#include <pcap.h>
#include <sys/stat.h>
#include <unistd.h>
#include <StrucioWriter.h>
#include <dts.h>

static struct smacq_options options[] = {
  {"f", {string_t:"-"}, "Output file", SMACQ_OPT_TYPE_STRING},
  {"s", {uint32_t:0}, "Maximum output file size (MB)", SMACQ_OPT_TYPE_UINT32},
  {"z", {boolean_t:0}, "Use gzip compression", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

class pcapwriteModule;

class StrucioPcapWriter : public StrucioWriter {
 public:
  StrucioPcapWriter(pcapwriteModule * o) { pcap = o; }
  void newfile_hook();

 private:
  pcapwriteModule * pcap;
};
    
SMACQ_MODULE(pcapwrite, 
  PROTO_CTOR(pcapwrite);
  PROTO_DTOR(pcapwrite);
  PROTO_CONSUME();

  void pcapwriteModule::fixup_pcap(struct old_pcap_pkthdr * hdr);
  void pcapwriteModule::parse_pcapfile(struct pcap_file_header * hdr);

  StrucioWriter * strucio;
  DtsObject datum;	
  int argc;
  char ** argv;
  int dts_pkthdr_type;		
  DtsObject current_datum;

  /* Booleans */
  bool do_produce;			/* Does this instance produce */
  bool swapped;
  bool extended;

  int hdr_size;
  struct pcap_file_header pcap_file_header;

  DtsObject snaplen_o;
  DtsObject linktype_o;
  DtsField snaplen_field;
  DtsField linktype_field;

  /* Const: */
  int snaplen_type;
  int linktype_type;
  int ifindex_type;
  int protocol_type; 
  int pkt_type_type;
  DtsField ifindex_field;
  DtsField protocol_field; 
  DtsField pkt_type_field;

  friend void StrucioPcapWriter::newfile_hook();
	     );

#define SWAPLONG(y) GUINT32_SWAP_LE_BE(y)
#define SWAPSHORT(y) GUINT16_SWAP_LE_BE(y)

static inline void swap_hdr(struct pcap_file_header *hp)
{
        hp->version_major = SWAPSHORT(hp->version_major);
        hp->version_minor = SWAPSHORT(hp->version_minor);
        hp->thiszone = SWAPLONG(hp->thiszone);
        hp->sigfigs = SWAPLONG(hp->sigfigs);
        hp->snaplen = SWAPLONG(hp->snaplen);
        hp->linktype = SWAPLONG(hp->linktype);
}

#define TCPDUMP_MAGIC 0xa1b2c3d4
#define TCPDUMP_MAGIC_NEW 0xa1b2cd34

void pcapwriteModule::parse_pcapfile(struct pcap_file_header * hdr) {
  swapped = 0;
  extended = 0;

  if (hdr->magic == TCPDUMP_MAGIC) {

  } else if (hdr->magic == SWAPLONG(TCPDUMP_MAGIC)) {
    swapped = 1;
  } else if (hdr->magic == TCPDUMP_MAGIC_NEW) {
    extended = 1;
  } else if (hdr->magic == SWAPLONG(TCPDUMP_MAGIC_NEW)) {
    extended = 1;
    swapped = 1;
  } else {
    fprintf(stderr, "bad dump file format");
    exit(-1);
  }

  if (extended) {
    hdr_size = sizeof(struct old_pcap_pkthdr) + sizeof(struct extended_pkthdr);
  } else {
    hdr_size = sizeof(struct old_pcap_pkthdr);
  }

  if (swapped)
    swap_hdr(hdr);

  if (hdr->version_major < PCAP_VERSION_MAJOR) {
    fprintf(stderr, "unsupported (old?) file format");
    exit(-1);
  }

  snaplen_o = dts->construct(snaplen_type, &pcap_file_header.snaplen);
  linktype_o = dts->construct(linktype_type, &pcap_file_header.linktype);
  assert(linktype_o);
  assert(snaplen_o);
}

void pcapwriteModule::fixup_pcap(struct old_pcap_pkthdr * hdr) {
  if (swapped) {
    hdr->caplen = SWAPLONG(hdr->caplen);
    hdr->len = SWAPLONG(hdr->len);
    hdr->ts.tv_sec = SWAPLONG(hdr->ts.tv_sec);
    hdr->ts.tv_usec = SWAPLONG(hdr->ts.tv_usec);
  }

  /*
   * We interchanged the caplen and len fields at version 2.3,
   * in order to match the bpf header layout.  But unfortunately
   * some files were written with version 2.3 in their headers
   * but without the interchanged fields.
   */
  if (pcap_file_header.version_minor < 3 ||
      (pcap_file_header.version_minor == 3 && hdr->caplen > hdr->len)) {
    int t = hdr->caplen;
    hdr->caplen = hdr->len;
    hdr->len = t;
  }
}

void StrucioPcapWriter::newfile_hook() {
  DtsObject linktype_o, snaplen_o;
  int linktype, snaplen;

  linktype_o = pcap->current_datum->getfield(pcap->linktype_field);
  if (linktype_o) {
    linktype = dts_data_as(linktype_o, int);
    
  } else {
    fprintf(stderr, "pcapfile: open: warning no linktype!\n");
    linktype = 1;
  }
  
  snaplen_o = pcap->current_datum->getfield(pcap->snaplen_field);
  if (snaplen_o) {
    snaplen = dts_data_as(snaplen_o, int);
    
  } else {
    fprintf(stderr, "pcapfile: open: warning no snaplen!\n");
    snaplen = 1514;
  }
  
  /*
  fprintf(stderr, "pcapfile: Info: Opening %s for output (linktype %d, snaplen %d)\n", 
	  filename, linktype, snaplen);
  */

  fprintf(stderr, "pcapfile: Info: Opening output (linktype %d, snaplen %d)\n", 
	  linktype, snaplen);
  
  pcap->pcap_file_header.snaplen = snaplen;
  pcap->pcap_file_header.linktype = linktype;

  if (-1 == this->write(&pcap->pcap_file_header, sizeof(pcap_file_header))) {
    perror("pcapfile write");
    assert(0);
  }
}

smacq_result pcapwriteModule::consume(DtsObject datum, int & outchan) {
  assert(datum);

  current_datum = datum;

  if (datum->gettype() == dts_pkthdr_type) {
    struct dts_pkthdr * pkt = (struct dts_pkthdr*)datum->getdata();

    if (-1 == strucio->write(pkt, sizeof(struct pcap_pkthdr) + pkt->pcap_pkthdr.caplen)) {
      return (smacq_result)(SMACQ_PASS|SMACQ_END|SMACQ_ERROR);
    }

  } else {
    fprintf(stderr, "Received unknown data type (expected packet)\n");
    exit(-1);
  }
	
  return SMACQ_PASS;
}

pcapwriteModule::~pcapwriteModule() {
  delete strucio;
}

pcapwriteModule::pcapwriteModule(struct SmacqModule::smacq_init * context) : SmacqModule(context) {
  smacq_opt output, size, gzip;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  dts->requiretype("packet");

  {
    struct smacq_optval optvals[] = {
      { "f", &output}, 
      { "s", &size}, 
      { "z", &gzip}, 
      {NULL, NULL}
    };
    output.uint32_t = 0;
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    
    strucio = new StrucioPcapWriter(this);

    strucio->register_filelist_args(argc, argv);

    if (size.uint32_t) {
      strucio->set_rotate(size.uint32_t * 1024 * 1024);
    }
    strucio->set_use_gzip(gzip.boolean_t);
  }

  snaplen_type = dts->requiretype("int");
  linktype_type = dts->requiretype("int");

  snaplen_field = dts->requirefield("snaplen");
  linktype_field = dts->requirefield("linktype");

  strucio->register_file(output.string_t);

  pcap_file_header.magic = TCPDUMP_MAGIC;
  pcap_file_header.version_major = 2;
  pcap_file_header.version_minor = 4;
  pcap_file_header.thiszone = 0;
  pcap_file_header.sigfigs = 0;

  dts_pkthdr_type = dts->requiretype("packet");
  assert(dts_pkthdr_type);
}

