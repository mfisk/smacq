#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include <smacq.h>
#include <dts_packet.h>
#include <pcap.h>
#include <zlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

enum file_read_type { EITHER, COPY, MAP };
static int open_file(struct state * state);
static inline void * read_current_file(struct state * state, void * buf, int len, enum file_read_type read_type);

static struct smacq_options options[] = {
  {"w", {string_t:"-"}, "Output file", SMACQ_OPT_TYPE_STRING},
  {"s", {uint32_t:0}, "Maximum output file size (MB)", SMACQ_OPT_TYPE_UINT32},
  {"l", {boolean_t:0}, "Read list of input files from STDIN", SMACQ_OPT_TYPE_BOOLEAN},
  {"z", {boolean_t:0}, "Use gzip compression", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

struct state {
  dts_object * datum;	
  smacq_environment * env;
  int argc;
  char ** argv;
  int dts_pkthdr_type;		

  /* Booleans */
  int produce;			/* Does this instance produce */
  int gzip;
  int swapped;
  int extended;
  int hdr_size;

  void * mmap;
  void * mmap_current;
  void * mmap_end;

  gzFile * gzfh;
  FILE * fh;
  struct pcap_file_header pcap_file_header;

  long long outputleft;
  unsigned long long maxfilesize;
  int suffix;
  int file_list;
  char * filename;

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

static inline void * read_current_file(struct state * state, void * buf, int len, enum file_read_type read_type) {
    if (state->mmap) {
      void * current = state->mmap_current;
      state->mmap_current += len;

      if (state->mmap_current > state->mmap_end) {
	if ((state->mmap_current - len) > state->mmap_end) {
	  fprintf(stderr, "pcapfile: premature end of mmapped file\n");
	}
	return NULL;
      }
      
      if (read_type == COPY) {
	memcpy(buf, current, len);
	return buf;
      } else {
	return current;
      }
    } else {
      int retval;
      assert(read_type != MAP);
      
      if (state->gzfh) {
	retval = gzread(state->gzfh, buf, len);
	//fprintf(stderr, "gzread returned %d\n", retval);
	return( (retval==1) ? buf : NULL );
      } else {
	retval = fread(buf, len, 1, state->fh);
	//fprintf(stderr, "fread returned %d on length %u\n", retval, len);
	return( (retval==1) ? buf : NULL );
      }
    }
}


static inline void close_file(struct state* state) {
  if (state->gzfh) {
    gzclose(state->gzfh);
    state->gzfh = NULL;
  }
  
  if (state->fh) {
    fclose(state->fh);
    state->fh = NULL;
  }
}

/* Read from this file or the next one */
static inline void * read_file(struct state * state, void * buf, int len, enum file_read_type read_type) {
  void * result;

  while (1) {
    result = read_current_file(state, buf, len, read_type);
    if (result) return result;

    close_file(state);
    if (!open_file(state)) {
      return NULL;
    } 
  }

}


static inline int open_filename(struct state * state, char * filename) {
  int try_mmap = 0;

  if ((!filename) || (!strcmp(filename, "-"))) {
    if (state->gzip) {
	    state->gzfh = gzdopen(0, "rb");
    } else {
	    state->fh = fdopen(0, "r");
    }
  } else {
    if (state->gzip) {
	    state->gzfh = gzopen(filename, "rb");
    } else {
	    state->fh = fopen(filename, "r");
	    try_mmap = 1;
    }
  }

  if (!state->fh && !state->gzfh) {
    perror("pcapfile open");
    exit(-1);
  }
  if (!read_file(state, &state->pcap_file_header, sizeof(struct pcap_file_header), COPY)) {
    perror("pcapfile read");
    exit(-1);
  }
  
  parse_pcapfile(state, &state->pcap_file_header);
  
  if (0 && try_mmap && !state->swapped && !state->extended) {
    struct stat stats;
    fstat(fileno(state->fh), &stats);

    state->mmap = mmap(NULL, stats.st_size, PROT_READ, MAP_SHARED, fileno(state->fh), 0);
    
    if (state->mmap) {
      state->mmap_end = state->mmap + stats.st_size;
      state->mmap_current = state->mmap + ftell(state->fh);
    } else {
      fprintf(stderr, "pcapfile: Error memory mapping file.  Will use stream I/O.\n");
    }
  }

  fprintf(stderr, "pcapfile: Opening %s for read ( ", filename);

  if (state->swapped) fprintf(stderr, " byte-swapped ");
  else fprintf(stderr, "host-byte-order ");

  if (state->extended) fprintf(stderr, " extended-header ");
  if (state->mmap) fprintf(stderr, " memory-mapped ");

  fprintf(stderr, ")\n");

  return(1); /* success */
}

static int open_file(struct state * state) {
  char * filename = "";
  char next_file[4096];

  if (state->file_list) {
	  int res;
	  filename = next_file;
	  res = scanf("%4096s", filename);
	  if (res == EOF) {
		  return(0);
	  } else if (res != 1) {
		  fprintf(stderr, "Error reading next filename from STDIN\n");
		  return(0);
	  }
  } else if (! state->argc) {
    //fprintf(stderr, "No more files to read from\n");
    return(0);
  } else {
    filename = state->argv[0];
    state->argc--; state->argv++;
  }

  return open_filename(state, filename);
}

static smacq_result pcapfile_produce(struct state * state, const dts_object ** datump, int * outchan) {
  struct old_pcap_pkthdr * hdrp;
  const dts_object * datum;
  struct dts_pkthdr * pkt;
  int res;

  if (!state->produce) return SMACQ_END;

  datum = smacq_alloc(state->env, state->pcap_file_header.snaplen + sizeof(struct dts_pkthdr) + sizeof(struct extended_pkthdr), 
		      state->dts_pkthdr_type);

  pkt = (struct dts_pkthdr*)dts_getdata(datum);

  hdrp = read_file(state, &pkt->pcap_pkthdr, state->hdr_size, EITHER);
  if (!hdrp) return SMACQ_END;

  fixup_pcap(state, hdrp);

  dts_incref(state->linktype_o, 1);
  dts_attach_field(datum, state->linktype_field, state->linktype_o);

  dts_incref(state->snaplen_o, 1);
  dts_attach_field(datum, state->snaplen_field, state->snaplen_o);

  if (hdrp == &pkt->pcap_pkthdr) {

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

    //fprintf(stderr, "reading packet of caplen %d\n", hdrp->caplen);
    if (!read_file(state, hdrp + 1, hdrp->caplen, COPY)) 
      return SMACQ_END;

  } else {
    void * payload; 

    ((dts_object *)datum)->data = hdrp;
    payload = read_file(state, NULL, hdrp->caplen, MAP);

    if (payload != hdrp +1) {
      fprintf(stderr, "payload at %p instead of %p\n", payload, hdrp + 1);
      fprintf(stderr, "pcapfile: premature end of file\n");
      return SMACQ_END;
    }
  }

  *datump = datum;

  return SMACQ_PASS|SMACQ_PRODUCE;
}

/* -1 iff error */
static int pcapfile_openwrite(struct state * state, const dts_object * datum) {
  char sufbuf[256];
  char * filename;
  const dts_object * linktype_o, * snaplen_o;
  int linktype, snaplen;
  
  if (! state->fh) {
    fprintf(stderr, "Output will be placed in pcapfile %s\n", state->filename);

    if (state->maxfilesize) {
      snprintf(sufbuf, 256, "%s.%02d", state->filename, state->suffix);
      filename = sufbuf;
      state->suffix++;
      
      state->outputleft = state->maxfilesize - sizeof(struct pcap_file_header);
    } else {
      filename = state->filename;
      state->outputleft = 1;
    }
    
    if (!strcmp(filename, "-")) {
      state->fh = stdout;
    } else {
      state->fh = fopen(filename, "w");
    }
    assert(state->fh);
  
    linktype_o = smacq_getfield(state->env, datum, state->linktype_field, NULL);
    if (linktype_o) {
      linktype = dts_data_as(linktype_o, int);
      dts_decref(linktype_o);
    } else {
      fprintf(stderr, "pcapfile: open: warning no linktype!\n");
      linktype = 1;
    }
    
    snaplen_o = smacq_getfield(state->env, datum, state->snaplen_field, NULL);
    if (snaplen_o) {
      snaplen = dts_data_as(snaplen_o, int);
      dts_decref(snaplen_o);
    } else {
      fprintf(stderr, "pcapfile: open: warning no snaplen!\n");
      snaplen = 1514;
    }
    
    fprintf(stderr, "pcapfile: Info: Opening %s for output (linktype %d, snaplen %d)\n", 
	    filename, linktype, snaplen);
    
    state->pcap_file_header.snaplen = snaplen;
    state->pcap_file_header.linktype = linktype;
    
    if (1 != fwrite(&state->pcap_file_header, sizeof(state->pcap_file_header), 1, state->fh)) {
      perror("pcapfile write");
      return -1;
    }
  }
  
}

static smacq_result pcapfile_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);

  if (dts_gettype(datum) == state->dts_pkthdr_type) {
	struct dts_pkthdr * pkt = dts_getdata(datum);

	state->outputleft -= (sizeof(struct pcap_pkthdr) + pkt->pcap_pkthdr.caplen);
	//fprintf(stderr, "%lld left\n", state->outputleft);
	if (state->maxfilesize && (state->outputleft <= 0)) {
	  fclose(state->fh);
	  state->fh = NULL;
	}

	if (-1 == pcapfile_openwrite(state, datum)) 
	  return(SMACQ_PASS|SMACQ_END|SMACQ_ERROR);

	if (0) {
	  int tot = fwrite(&pkt->pcap_pkthdr, sizeof(pkt->pcap_pkthdr), 1, state->fh);
	  //tot += fwrite(&pkt->extended, sizeof(pkt->extended), 1, state->fh);
	  tot += fwrite(pkt+1, pkt->pcap_pkthdr.caplen, 1, state->fh);
	  if (tot != 3) {
		perror("pcapfile write");
		return(SMACQ_PASS|SMACQ_END|SMACQ_ERROR);
	  }
	} else {
	  if (1 != fwrite(&pkt->pcap_pkthdr, sizeof(pkt->pcap_pkthdr) + pkt->pcap_pkthdr.caplen, 1, state->fh)) {
		perror("pcapfile write");
		return(SMACQ_PASS|SMACQ_END|SMACQ_ERROR);
	  }
	};

  } else {
	fprintf(stderr, "Received unknown data type (expected packet)\n");
	exit(-1);
  }
	
  return SMACQ_PASS;
}

static smacq_result pcapfile_shutdown(struct state * state) {
	close_file(state);
	free(state);

	return 0;
}

static smacq_result pcapfile_init(struct smacq_init * context) {
  struct state * state;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  smacq_requiretype(context->env, "packet");

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    smacq_opt output, size, list, gzip;

    struct smacq_optval optvals[] = {
      { "w", &output}, 
      { "s", &size}, 
      { "l", &list}, 
      { "z", &gzip}, 
      {NULL, NULL}
    };
    output.uint32_t = 0;
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    
    state->filename = output.string_t;
    state->file_list = list.boolean_t;
    state->maxfilesize = size.uint32_t * 1024 * 1024;
    state->outputleft = 1024*1024;
    state->gzip = gzip.boolean_t;
  }
  
  state->snaplen_type = smacq_requiretype(state->env, "int");
  state->linktype_type = smacq_requiretype(state->env, "int");

  state->snaplen_field = smacq_requirefield(state->env, "snaplen");
  state->linktype_field = smacq_requirefield(state->env, "linktype");

  if (context->isfirst) {
    //fprintf(stderr, "Reading pcapfile (no predecessor)\n");
    state->produce = 1;

    state->ifindex_type = smacq_requiretype(state->env, "int");
    state->protocol_type = smacq_requiretype(state->env, "ushort");
    state->pkt_type_type = smacq_requiretype(state->env, "ubyte");

    state->ifindex_field = smacq_requirefield(state->env, "ifindex");
    state->protocol_field = smacq_requirefield(state->env, "ethertype");
    state->pkt_type_field = smacq_requirefield(state->env, "pkt_type");

    open_file(state);
  } else if (context->islast) {
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
  &pcapfile_produce, 
  &pcapfile_consume,
  &pcapfile_init,
  &pcapfile_shutdown
};

