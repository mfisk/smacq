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

static struct smacq_options options[] = {
  {"w", {string_t:"-"}, "Output file", SMACQ_OPT_TYPE_STRING},
  {"s", {uint32_t:0}, "Maximum output file size (MB)", SMACQ_OPT_TYPE_UINT32},
  {"l", {boolean_t:0}, "Read list of input files from STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"z", {boolean_t:0}, "Use gzip compression", SMACQ_OPT_TYPE_BOOLEAN},
  {"M", {boolean_t:0}, "Disable memory-mapped I/O", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  dts_object * datum;	
  smacq_environment * env;
  int argc;
  char ** argv;
  int dts_pkthdr_type;		
  struct strucio * rdr;
  const dts_object * current_datum;

  /* Booleans */
  int produce;			/* Does this instance produce */
  int swapped;
  int extended;
  int hdr_size;
  struct pcap_file_header pcap_file_header;

  const dts_object * snaplen_o, * linktype_o;
  dts_field snaplen_field, linktype_field;

  /* Const: */
  int snaplen_type, linktype_type;
  int ifindex_type, protocol_type, pkt_type_type;
  dts_field ifindex_field, protocol_field, pkt_type_field;
};


#define SWAPLONG(y) \
((((y)&0xff)<<24) | (((y)&0xff00)<<8) | (((y)&0xff0000)>>8) | (((y)>>24)&0xff))
#define SWAPSHORT(y) \
        ( (((y)&0xff)<<8) | ((u_short)((y)&0xff00)>>8) )


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

static inline void parse_pcapfile(struct state * state, struct pcap_file_header * hdr) {
  state->swapped = 0;
  state->extended = 0;

  if (hdr->magic == TCPDUMP_MAGIC) {

  } else if (hdr->magic == SWAPLONG(TCPDUMP_MAGIC)) {
    state->swapped = 1;
  } else if (hdr->magic == TCPDUMP_MAGIC_NEW) {
    state->extended = 1;
  } else if (hdr->magic == SWAPLONG(TCPDUMP_MAGIC_NEW)) {
    state->extended = 1;
    state->swapped = 1;
  } else {
    fprintf(stderr, "bad dump file format");
    exit(-1);
  }

  if (state->extended) {
    state->hdr_size = sizeof(struct old_pcap_pkthdr) + sizeof(struct extended_pkthdr);
  } else {
    state->hdr_size = sizeof(struct old_pcap_pkthdr);
  }

  if (state->swapped)
    swap_hdr(hdr);

  if (hdr->version_major < PCAP_VERSION_MAJOR) {
    fprintf(stderr, "unsupported (old?) file format");
    exit(-1);
  }

  if (state->snaplen_o) dts_decref(state->snaplen_o);
  if (state->linktype_o) dts_decref(state->linktype_o);
  state->snaplen_o = smacq_dts_construct(state->env, state->snaplen_type, &state->pcap_file_header.snaplen);
  state->linktype_o = smacq_dts_construct(state->env, state->linktype_type, &state->pcap_file_header.linktype);
  assert(state->linktype_o);
  assert(state->snaplen_o);
}

static inline void fixup_pcap(struct state * state, struct old_pcap_pkthdr * hdr) {
  if (state->swapped) {
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
  if (state->pcap_file_header.version_minor < 3 ||
      (state->pcap_file_header.version_minor == 3 && hdr->caplen > hdr->len)) {
    int t = hdr->caplen;
    hdr->caplen = hdr->len;
    hdr->len = t;
  }
}

int newfile_read(struct strucio * rdr, void * data) {
  struct state * state = data;

  if (!strucio_read_copy(rdr, &state->pcap_file_header, sizeof(struct pcap_file_header))) {
    perror("pcapfile read");
    exit(-1);
  }
  
  parse_pcapfile(state, &state->pcap_file_header);

  //  fprintf(stderr, "pcapfile: Opening %s for read ( ", state->rdr->filename);
  fprintf(stderr, "pcapfile: Opening for read ( ");

  if (state->swapped) fprintf(stderr, "byte-swapped ");
  else fprintf(stderr, "host-byte-order ");

  if (state->extended) fprintf(stderr, "extended-header ");
  //  if (state->rdr->mmap) fprintf(stderr, "memory-mapped ");

  fprintf(stderr, ")\n");

  return 0;
}

static smacq_result pcapfile_produce(struct state * state, const dts_object ** datump, int * outchan) {
  struct old_pcap_pkthdr * hdrp;
  const dts_object * datum;
  struct dts_pkthdr * pkt;

  if (!state->produce) return SMACQ_END;

  datum = smacq_alloc(state->env, state->pcap_file_header.snaplen + sizeof(struct dts_pkthdr) + sizeof(struct extended_pkthdr), 
		      state->dts_pkthdr_type);

  pkt = (struct dts_pkthdr*)dts_getdata(datum);

  hdrp = strucio_read(state->rdr, &pkt->pcap_pkthdr, state->hdr_size);
  if (!hdrp) return SMACQ_END;

  fixup_pcap(state, hdrp);

  dts_incref(state->linktype_o, 1);
  dts_attach_field(datum, state->linktype_field, state->linktype_o);

  dts_incref(state->snaplen_o, 1);
  dts_attach_field(datum, state->snaplen_field, state->snaplen_o);

  if (state->extended) {
    const dts_object * newo;
    struct extended_pkthdr * ehdr = (struct extended_pkthdr*)(hdrp+1);
    
    newo = smacq_dts_construct(state->env, state->ifindex_type, &ehdr->ifindex);
    dts_attach_field(datum, state->ifindex_field, newo);
    
    newo = smacq_dts_construct(state->env, state->protocol_type, &ehdr->protocol);
    dts_attach_field(datum, state->protocol_field, newo);
    
    newo = smacq_dts_construct(state->env, state->pkt_type_type, &ehdr->pkt_type);
    dts_attach_field(datum, state->pkt_type_field, newo);
  }
  
  if (hdrp == &pkt->pcap_pkthdr) {
    //fprintf(stderr, "reading packet of caplen %d\n", hdrp->caplen);
    if (!strucio_read_copy(state->rdr, hdrp + 1, hdrp->caplen)) 
      return SMACQ_END;

  } else {
    void * payload; 

    ((dts_object *)datum)->data = hdrp;
    ((dts_object *)datum)->len = sizeof(struct dts_pkthdr) + hdrp->caplen;

    payload = strucio_read_mmap(state->rdr, hdrp->caplen);

    if (payload != hdrp +1) {
      //fprintf(stderr, "payload at %p instead of %p\n", payload, hdrp + 1);
      fprintf(stderr, "pcapfile: premature end of mmapped file\n");
      return SMACQ_END;
    }
  }

  *datump = datum;

  return SMACQ_PASS|SMACQ_PRODUCE;
}

int newfile_write(struct strucio * rdr, void * data) {
  struct state * state = data;
  const dts_object * linktype_o, * snaplen_o;
  int linktype, snaplen;

  linktype_o = smacq_getfield(state->env, state->current_datum, state->linktype_field, NULL);
  if (linktype_o) {
    linktype = dts_data_as(linktype_o, int);
    dts_decref(linktype_o);
  } else {
    fprintf(stderr, "pcapfile: open: warning no linktype!\n");
    linktype = 1;
  }
  
  snaplen_o = smacq_getfield(state->env, state->current_datum, state->snaplen_field, NULL);
  if (snaplen_o) {
    snaplen = dts_data_as(snaplen_o, int);
    dts_decref(snaplen_o);
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
  
  state->pcap_file_header.snaplen = snaplen;
  state->pcap_file_header.linktype = linktype;

  if (-1 == strucio_write(state->rdr, &state->pcap_file_header, sizeof(state->pcap_file_header))) {
    perror("pcapfile write");
    return -1;
  }
 
  return 0;
}

static smacq_result pcapfile_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);

  state->current_datum = datum;

  if (dts_gettype(datum) == state->dts_pkthdr_type) {
    struct dts_pkthdr * pkt = dts_getdata(datum);

    if (-1 == strucio_write(state->rdr, pkt, sizeof(struct pcap_pkthdr) + pkt->pcap_pkthdr.caplen)) {
      return(SMACQ_PASS|SMACQ_END|SMACQ_ERROR);
    }

  } else {
    fprintf(stderr, "Received unknown data type (expected packet)\n");
    exit(-1);
  }
	
  return SMACQ_PASS;
}

static smacq_result pcapfile_shutdown(struct state * state) {
  strucio_close(state->rdr);
  free(state);
  
  return 0;
}

static smacq_result pcapfile_init(struct smacq_init * context) {
  struct state * state;
  smacq_opt output, size, list, gzip, avoid_mmap;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  smacq_requiretype(context->env, "packet");

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->rdr = strucio_init();
  state->env = context->env;
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
				 &state->argc, &state->argv,
				 options, optvals);
    
    if (list.boolean_t) {
      strucio_register_filelist_stdin(state->rdr);
    } else {
      strucio_register_filelist_args(state->rdr, state->argc, state->argv);
    }

    if (size.uint32_t) {
      strucio_set_rotate(state->rdr, size.uint32_t * 1024 * 1024);
    }
    if (avoid_mmap.boolean_t) {
      strucio_set_read_type(state->rdr, COPY);
    }
    strucio_set_use_gzip(state->rdr, gzip.boolean_t);
  }

  state->snaplen_type = smacq_requiretype(state->env, "int");
  state->linktype_type = smacq_requiretype(state->env, "int");

  state->snaplen_field = smacq_requirefield(state->env, "snaplen");
  state->linktype_field = smacq_requirefield(state->env, "linktype");

  if (context->isfirst) {
    strucio_register_newfile(state->rdr, newfile_read, state);
    //fprintf(stderr, "Reading pcapfile (no predecessor)\n");
    state->produce = 1;

    state->ifindex_type = smacq_requiretype(state->env, "int");
    state->protocol_type = smacq_requiretype(state->env, "ushort");
    state->pkt_type_type = smacq_requiretype(state->env, "ubyte");

    state->ifindex_field = smacq_requirefield(state->env, "ifindex");
    state->protocol_field = smacq_requirefield(state->env, "ethertype");
    state->pkt_type_field = smacq_requirefield(state->env, "pkt_type");
    assert(!strucio_open(state->rdr));
  } else if (context->islast) {
    strucio_register_newfile(state->rdr, newfile_write, state);
    strucio_register_file(state->rdr, output.string_t);

    state->pcap_file_header.magic = TCPDUMP_MAGIC;
    state->pcap_file_header.version_major = 2;
    state->pcap_file_header.version_minor = 4;
    state->pcap_file_header.thiszone = 0;
    state->pcap_file_header.sigfigs = 0;
  } else {
    fprintf(stderr, "pcapfile module must be at beginning or end of dataflow\n");
    exit(-1);
  }

  state->dts_pkthdr_type = smacq_requiretype(context->env, "packet");
  assert(state->dts_pkthdr_type);

  return 0;
}

/* Right now this serves mainly for type checking at compile time: */
struct smacq_functions smacq_pcapfile_table = {
  produce: &pcapfile_produce, 
  consume: &pcapfile_consume,
  init: &pcapfile_init,
  shutdown: &pcapfile_shutdown
};

