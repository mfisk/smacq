#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include <smacq.h>
#include <dts_packet.h>
#include <pcap.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strucio.h>
#include <dts.h>

static struct smacq_options options[] = {
  {"w", {string_t:"-"}, "Output file", SMACQ_OPT_TYPE_STRING},
  {"s", {uint32_t:0}, "Maximum output file size (MB)", SMACQ_OPT_TYPE_UINT32},
  {"l", {boolean_t:0}, "Read list of input files from STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"z", {boolean_t:0}, "Use gzip compression", SMACQ_OPT_TYPE_BOOLEAN},
  {"M", {boolean_t:0}, "Disable memory-mapped I/O", SMACQ_OPT_TYPE_BOOLEAN},
  END_SMACQ_OPTIONS
};

class pcapfileModule;

class StrucioReader : public Strucio {
 public:
  StrucioReader(pcapfileModule * o) { pcap = o; }
 
 private: 
  pcapfileModule * pcap;
  void newfile_hook();
};

class StrucioWriter : public Strucio {
 public:
  StrucioWriter(pcapfileModule * o) { pcap = o; }

 private:
  pcapfileModule * pcap;
  void newfile_hook();
};
    
SMACQ_MODULE(pcapfile, 
  PROTO_CTOR(pcapfile);
  PROTO_DTOR(pcapfile);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  void pcapfileModule::fixup_pcap(struct old_pcap_pkthdr * hdr);
  void pcapfileModule::parse_pcapfile(struct pcap_file_header * hdr);

  Strucio * strucio;
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
  dts_field snaplen_field;
  dts_field linktype_field;

  /* Const: */
  int snaplen_type;
  int linktype_type;
  int ifindex_type;
  int protocol_type; 
  int pkt_type_type;
  dts_field ifindex_field;
  dts_field protocol_field; 
  dts_field pkt_type_field;

  friend void StrucioReader::newfile_hook();
  friend void StrucioWriter::newfile_hook();
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

void pcapfileModule::parse_pcapfile(struct pcap_file_header * hdr) {
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

void pcapfileModule::fixup_pcap(struct old_pcap_pkthdr * hdr) {
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

void StrucioReader::newfile_hook() {
  if (!read_copy(&pcap->pcap_file_header, sizeof(struct pcap_file_header))) {
    perror("pcapfile read");
    exit(-1);
  }
  
  pcap->parse_pcapfile(&pcap->pcap_file_header);

  //  fprintf(stderr, "pcapfile: Opening %s for read ( ", filename);
  fprintf(stderr, "pcapfile: Opening for read ( ");

  if (pcap->swapped) fprintf(stderr, "byte-swapped ");
  else fprintf(stderr, "host-byte-order ");

  if (pcap->extended) fprintf(stderr, "extended-header ");
  //  if (mmap) fprintf(stderr, "memory-mapped ");

  fprintf(stderr, ")\n");
}

smacq_result pcapfileModule::produce(DtsObject & datum, int * outchan) {
  struct old_pcap_pkthdr * hdrp;
  struct dts_pkthdr * pkt;

  if (!do_produce) return SMACQ_END;
  
  datum = dts->newObject(dts_pkthdr_type, 
			 pcap_file_header.snaplen 
			 + sizeof(struct dts_pkthdr) 
			 + sizeof(struct extended_pkthdr));

  pkt = (struct dts_pkthdr*)datum->getdata();

  hdrp = (old_pcap_pkthdr*)strucio->read(&pkt->pcap_pkthdr, hdr_size);
  if (!hdrp) {
	return SMACQ_END;
  }

  fixup_pcap(hdrp);

  
  datum->attach_field(linktype_field, linktype_o);

  
  datum->attach_field(snaplen_field, snaplen_o);

  if (extended) {
    DtsObject newo;
    struct extended_pkthdr * ehdr = (struct extended_pkthdr*)(hdrp+1);
    
    newo = dts->construct(ifindex_type, &ehdr->ifindex);
    datum->attach_field(ifindex_field, newo);
    
    newo = dts->construct(protocol_type, &ehdr->protocol);
    datum->attach_field(protocol_field, newo);
    
    newo = dts->construct(pkt_type_type, &ehdr->pkt_type);
    datum->attach_field(pkt_type_field, newo);
  }
  
  if (hdrp == &pkt->pcap_pkthdr) {
    //fprintf(stderr, "reading packet of caplen %d\n", hdrp->caplen);
    if (!strucio->read_copy(hdrp + 1, hdrp->caplen)) {
      fprintf(stderr, "pcapfile: Error: Premature end of file\n");
      return SMACQ_END;
    }

  } else {
    void * payload; 

    datum->setdata(hdrp);
    datum->setsize(sizeof(struct dts_pkthdr) + hdrp->caplen);

    payload = strucio->read_mmap(hdrp->caplen);

    if (payload != hdrp +1) {
      fprintf(stderr, "payload at %p instead of %p\n", payload, hdrp + 1);
      fprintf(stderr, "pcapfile: premature end of mmapped file\n");
      return SMACQ_END;
    }
  }

  return (smacq_result)(SMACQ_PASS|SMACQ_PRODUCE);
}

void StrucioWriter::newfile_hook() {
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

smacq_result pcapfileModule::consume(DtsObject datum, int * outchan) {
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

pcapfileModule::~pcapfileModule() {
  delete strucio;
}

pcapfileModule::pcapfileModule(struct smacq_init * context) : SmacqModule(context) {
  smacq_opt output, size, list, gzip, avoid_mmap;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  dts->requiretype("packet");

  {
    struct smacq_optval optvals[] = {
      { "w", &output}, 
      { "s", &size}, 
      { "l", &list}, 
      { "z", &gzip}, 
      { "M", &avoid_mmap}, 
      {NULL, NULL}
    };
    output.uint32_t = 0;
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
    
    if (context->isfirst) {
      strucio = new StrucioReader(this);
    } else {
      strucio = new StrucioWriter(this);
    }

    if (list.boolean_t) {
      strucio->register_filelist_stdin();
    } else {
      strucio->register_filelist_args(argc, argv);
    }

    if (size.uint32_t) {
      strucio->set_rotate(size.uint32_t * 1024 * 1024);
    }
    if (avoid_mmap.boolean_t) {
      strucio->set_read_type(COPY);
    }
    strucio->set_use_gzip(gzip.boolean_t);
  }

  snaplen_type = dts->requiretype("int");
  linktype_type = dts->requiretype("int");

  snaplen_field = dts->requirefield("snaplen");
  linktype_field = dts->requirefield("linktype");

  if (context->isfirst) {
    //fprintf(stderr, "Reading pcapfile (no predecessor)\n");
    do_produce = 1;

    ifindex_type = dts->requiretype("int");
    protocol_type = dts->requiretype("ushort");
    pkt_type_type = dts->requiretype("ubyte");

    ifindex_field = dts->requirefield("ifindex");
    protocol_field = dts->requirefield("ethertype");
    pkt_type_field = dts->requirefield("pkt_type");
  } else if (context->islast) {
    strucio->register_file(output.string_t);

    pcap_file_header.magic = TCPDUMP_MAGIC;
    pcap_file_header.version_major = 2;
    pcap_file_header.version_minor = 4;
    pcap_file_header.thiszone = 0;
    pcap_file_header.sigfigs = 0;
  } else {
    fprintf(stderr, "pcapfile module must be at beginning or end of dataflow\n");
    exit(-1);
  }

  dts_pkthdr_type = dts->requiretype("packet");
  assert(dts_pkthdr_type);

    /* Strucio class makes open() optional, but we need to open()
       before the first read() so we know how much to read. */
  strucio->open() ;
}

