#ifdef linux
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <netinet/if_ether.h>
#include "smacq.h"
#include "pcapfile.h"
#include "dts_packet.h"

static struct smacq_options options[] = {
  {"o", {string_t:"-"}, "Output file", SMACQ_OPT_TYPE_STRING},
  {"s", {uint32_t:0}, "Maximum output file size (MB)", SMACQ_OPT_TYPE_UINT32},
  {NULL, {string_t:NULL}, NULL, 0}
};

static int open_file(struct state * state) {
  gzFile * filep;

  if (! state->argc) {
    //fprintf(stderr, "No more files to read from\n");
    return(0);
  }

  if (state->pcap) pcap_close(state->pcap);
  fprintf(stderr, "pcapfile: Opening %s for read\n", state->argv[0]);
  
  if ((!state->argv[0]) || (!strcmp(state->argv[0], "-"))) {
    filep = gzdopen(0, "rb");
  } else {
    filep = gzopen(state->argv[0], "rb");
  }

  if (!filep) {
    perror("open");
    exit(-1);
  }
  if (1 > gzread(filep, &state->pcap_file_header, sizeof(struct pcap_file_header))) {
    perror("read");
    exit(-1);
  }
  
  parse_pcapfile(state, &state->pcap_file_header);

  state->argc--; state->argv++;
  state->gzfile = filep;

  return(1); /* success */
}

/* Read from this file or the next one */
static int read_file(struct state * state, void * buf, int len) {
  int retval;

  while (1) {
    retval = gzread(state->gzfile, buf, len);
    if (retval == 0) {
      if (open_file(state)) {
	continue; /* Try again with this file */
      } else {
	return 0;
      }
    } else if (retval == -1) {
      perror("read");
      return -1;
    } else {
      return retval;
    }
  }
}

static smacq_result pcapfile_produce(struct state * state, const dts_object ** datump, int * outchan) {
  struct old_pcap_pkthdr hdr;
  struct dts_pkthdr * pkt;
  const dts_object * datum = NULL;

  if (!state->produce) return SMACQ_END;

  assert(state->gzfile);

  if (sizeof(struct old_pcap_pkthdr) != read_file(state, &hdr, sizeof(struct old_pcap_pkthdr))) 
    return SMACQ_END;

  fixup_pcap(state, &hdr);

  datum = flow_alloc(state->env, hdr.caplen + sizeof(struct dts_pkthdr), 
				state->dts_pkthdr_type);
  pkt = (struct dts_pkthdr*)dts_getdata(datum);

  pkt->pcap_pkthdr = hdr;
  pkt->linktype = state->pcap_file_header.linktype;
  pkt->snaplen = state->pcap_file_header.snaplen;

  if (state->extended) {
    if (sizeof(struct extended_pkthdr) != read_file(state, &pkt->extended, sizeof(struct extended_pkthdr))) 
      return SMACQ_END;
  } else {
    memset(&pkt->extended, 0, sizeof(struct extended_pkthdr));
  }

  if (1 > gzread(state->gzfile, dts_getdata(datum)+sizeof(struct dts_pkthdr), hdr.caplen)) 
    return SMACQ_END;

  *datump = datum;

  return SMACQ_PASS|SMACQ_PRODUCE;
}

static smacq_result pcapfile_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(datum);

  if (dts_gettype(datum) == state->dts_pkthdr_type) {
	struct dts_pkthdr * pkt = dts_getdata(datum);

	state->outputleft -= (sizeof(struct pcap_pkthdr) + pkt->pcap_pkthdr.caplen);
	//fprintf(stderr, "%lld left\n", state->outputleft);
	if (state->maxfilesize && (state->outputleft <= 0)) {
	  pcap_close(state->pcap);
	  state->pcap = NULL;
	}

	if (! state->pcap) {
	  char sufbuf[256];
	  char * filename;

	  if (state->maxfilesize) {
	    snprintf(sufbuf, 256, "%s.%02d", state->opts.output, state->suffix);
	    filename = sufbuf;
	    state->suffix++;
	    
	    state->outputleft = state->maxfilesize - sizeof(struct pcap_file_header);
	   } else {
	     filename = state->opts.output;
	     state->outputleft = 1;
	   }

	  state->pcap=pcap_open_dead(pkt->linktype, pkt->snaplen);
	  assert(state->pcap);
	  
	  state->dumper=pcap_dump_open(state->pcap, filename);
	  fprintf(stderr, "pcapfile: Info: Opening %s for output (linktype %d, snaplen %d)\n", filename, pkt->linktype, pkt->snaplen);
	  assert(state->dumper);
	}
	  
	pcap_dump((char*)state->dumper, (struct pcap_pkthdr*)&pkt->pcap_pkthdr, dts_getdata(datum)+sizeof(struct dts_pkthdr));

  } else {
	fprintf(stderr, "Received unknown structure type\n");
	exit(-1);
  }
	
  return SMACQ_PASS;
}

static int pcapfile_shutdown(struct state * state) {
	if (state->dumper) {
	  pcap_dump_close(state->dumper);
	  state->dumper = NULL;
	}
	if (state->pcap) {
	  pcap_close(state->pcap);
	  state->pcap = NULL;
	}
	if (state->gzfile) {
	  gzclose(state->gzfile);
	  state->gzfile = NULL;
	}

	free(state);

	return 0;
}

static int pcapfile_init(struct flow_init * context) {
  struct state * state;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  flow_requiretype(context->env, "packet");

  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);

  state->env = context->env;
  {
    smacq_opt output, size;

    struct smacq_optval optvals[] = {
      { "o", &output}, 
      { "s", &size}, 
      {NULL, NULL}
    };
    output.uint32_t = 0;
    flow_getoptsbyname(context->argc-1, context->argv+1,
				 &state->argc, &state->argv,
				 options, optvals);
    
    state->opts.output = output.string_t;
    state->maxfilesize = size.uint32_t * 1024 * 1024;
    state->outputleft = 1024*1024;
  }

  if (context->isfirst) {
    //fprintf(stderr, "Reading pcapfile (no predecessor)\n");
    if (context->islast) {
      fprintf(stderr, "Error: nobody to produce for!\n");
      exit(-1);
    }
    open_file(state);
    state->produce = 1;
  } else if (context->islast) {
    fprintf(stderr, "Writing pcapfile %s\n", state->opts.output);
  } else {
    fprintf(stderr, "pcapfile module must be at beginning or end of dataflow\n");
    exit(-1);
  }

  state->dts_pkthdr_type = smacq_opt_typenum_byname(context->env, "packet");
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

