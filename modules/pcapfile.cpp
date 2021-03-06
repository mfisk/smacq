#define SMACQ_MODULE_IS_STATELESS 1
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <StrucioStream.h>
#include <SmacqModule.h>
#include <dts.h>

#include <dts_packet.h>

SMACQ_MODULE(pcapfile, 
  PROTO_CTOR(pcapfile);
  PROTO_CONSUME();
  PROTO_PRODUCE();
  PROTO_DTOR(pcapfile);

  void fixup_pcap(struct old_pcap_pkthdr * hdr);
  void parse_pcapfile(struct pcap_file_header * hdr);
  void parse_packets(StrucioStream * fh);

  dts_typeid dts_pkthdr_type;		

  /* Booleans */
  bool swapped;
  bool extended;

  StrucioStream * fh;

  unsigned int hdr_size;
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
    fprintf(stderr, "bad dump file format\n");
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

smacq_result pcapfileModule::consume(DtsObject fileo, int & outchan) {
  if (fh) delete fh; // Clean-up previous file

  fh = StrucioStream::MagicOpen(fileo);

  if (!fh) {
    return SMACQ_FREE;
  }

  if (sizeof(struct pcap_file_header) != fh->Read(&pcap_file_header, sizeof(struct pcap_file_header))) {
    perror("pcapfileModule");
    exit(-1);
  }
  
  parse_pcapfile(&pcap_file_header);

  //  fprintf(stderr, "pcapfile: Opening %s for read ( ", filename);
  fprintf(stderr, "pcapfile: Opening file %s ( ", StrucioStream::Filename(fileo).c_str());

  if (swapped) fprintf(stderr, "byte-swapped ");
  else fprintf(stderr, "host-byte-order ");

  if (extended) fprintf(stderr, "extended-header ");

  fprintf(stderr, ")\n");

  return SMACQ_FREE|SMACQ_PRODUCE;
}

smacq_result pcapfileModule::produce(DtsObject &datum, int & outchan) {
    if (!fh) return SMACQ_FREE;

    struct old_pcap_pkthdr * hdrp;
    struct dts_pkthdr * pkt;

    datum  = dts->newObject(dts_pkthdr_type, pcap_file_header.snaplen + hdr_size);

    pkt = (struct dts_pkthdr*)datum->getdata();

    hdrp = &pkt->pcap_pkthdr;
    if (hdr_size != fh->Read(hdrp, hdr_size)) {
	// fprintf(stderr, "pcapfile: error reading packet header\n");  // Probably normal EOF
	return SMACQ_FREE;
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
  
    //fprintf(stderr, "reading packet of caplen %d\n", hdrp->caplen);
    if (hdrp->caplen != fh->Read(hdrp + 1, hdrp->caplen)) {
      fprintf(stderr, "pcapfile: Error: Premature end of file\n");
      return SMACQ_FREE;
    }

    // Shrink size to caplen plus metadata
    datum->setsize(hdrp->caplen + sizeof(*hdrp));
  
    return SMACQ_PASS|SMACQ_PRODUCE;
}

pcapfileModule::pcapfileModule(struct SmacqModule::smacq_init * context) : SmacqModule(context), fh(NULL) {
  SmacqFileModule(context);

  snaplen_type = dts->requiretype("int");
  linktype_type = dts->requiretype("int");

  snaplen_field = dts->requirefield("snaplen");
  linktype_field = dts->requirefield("linktype");

  ifindex_type = dts->requiretype("int");
  protocol_type = dts->requiretype("ushort");
  pkt_type_type = dts->requiretype("ubyte");

  ifindex_field = dts->requirefield("ifindex");
  protocol_field = dts->requirefield("ethertype");
  pkt_type_field = dts->requirefield("pkt_type");

  dts_pkthdr_type = dts->requiretype("packet");
  assert(dts_pkthdr_type);
}

pcapfileModule::~pcapfileModule() {
  if (fh) delete fh;
}
