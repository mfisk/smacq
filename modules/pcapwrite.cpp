#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include <SmacqModule.h>
#include <FieldVec.h>
#include <dts_packet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <StrucioWriter.h>
#include <dts.h>
#include <boost/format.hpp>

static struct smacq_options options[] = {
  {"f", {string_t:"-"}, "Output file", SMACQ_OPT_TYPE_STRING},
  {"s", {uint32_t:0}, "Maximum output file size (MB)", SMACQ_OPT_TYPE_UINT32},
  {"t", {uint32_t:0}, "Maximum output file time (seconds)", SMACQ_OPT_TYPE_UINT32},
  {"z", {boolean_t:0}, "Use gzip compression", SMACQ_OPT_TYPE_BOOLEAN},
  {"b", {uint32_t:0}, "Number of buckets to hash packets into", SMACQ_OPT_TYPE_UINT32},
  END_SMACQ_OPTIONS
};

class pcapwriteModule;

class StrucioPcapWriter : public StrucioWriter {
 public:
  StrucioPcapWriter() : pcap(NULL) {}
  //StrucioPcapWriter(const StrucioPcapWriter & s) : pcap(NULL) { newFilelist(NULL); }
  void newfile_hook();
  void setPcap(pcapwriteModule * o) { pcap = o; }

 private:
  pcapwriteModule * pcap;
};
    
SMACQ_MODULE(pcapwrite, 
  PROTO_CTOR(pcapwrite);
  PROTO_DTOR(pcapwrite);
  PROTO_CONSUME();

  std::vector<StrucioPcapWriter>strucio;
  DtsObject datum;	
  int argc;
  const char ** argv;
  int dts_pkthdr_type;		
  DtsObject current_datum;
  int buckets;
  uint32_t rotate_time;

  /* Booleans */
  struct pcap_file_header pcap_file_header;

  DtsObject snaplen_o;
  DtsObject linktype_o;
  DtsField snaplen_field;
  DtsField linktype_field;
  DtsField ts_field;
  DtsField uint32_field;

  FieldVec fieldvec;

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

#define TCPDUMP_MAGIC 0xa1b2c3d4
#define TCPDUMP_MAGIC_NEW 0xa1b2cd34

/// Called after each new file is created to write out per-file header.
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

  if (!this->write(&pcap->pcap_file_header, sizeof(pcap_file_header))) {
    perror("pcapfile write");
    assert(0);
  }
}

smacq_result pcapwriteModule::consume(DtsObject datum, int & outchan) {
  assert(datum);
  int b;

  if (buckets > 1) {
    fieldvec.getfields(datum);
    b = fieldvec.getobjs().sethash() % buckets;
    //fprintf(stderr, "Bucket %d\n", b);
  } else {
    b = 0;
  }

  current_datum = datum;

  if (datum->gettype() == dts_pkthdr_type) {
    struct dts_pkthdr * pkt = (struct dts_pkthdr*)datum->getdata();
    uint32_t now = 0;

    if (rotate_time) {
      DtsObject tf = datum->getfield(ts_field);
      if (tf) {
        tf = tf->getfield(uint32_field);
        if (tf) {
          now = dts_data_as(tf, uint32_t);
        }
      }
    }
    if (!strucio[b].write(pkt, sizeof(struct old_pcap_pkthdr) + pkt->pcap_pkthdr.caplen, now)) {
      return (smacq_result)(SMACQ_PASS|SMACQ_END|SMACQ_ERROR);
    }

  } else {
    fprintf(stderr, "Received unknown data type (expected packet)\n");
    exit(-1);
  }
	
  return SMACQ_PASS;
}

pcapwriteModule::~pcapwriteModule() {
}

pcapwriteModule::pcapwriteModule(struct SmacqModule::smacq_init * context) : SmacqModule(context), rotate_time(0) {
  smacq_opt output, rotate_size, rotate_time_opt, gzip, buckets_opt;

  //fprintf(stderr, "Loading pcapfile (%d,%d)\n", context->isfirst, context->islast);
  dts->requiretype("packet");

  {
    struct smacq_optval optvals[] = {
      { "f", &output}, 
      { "s", &rotate_size}, 
      { "t", &rotate_time_opt}, 
      { "z", &gzip}, 
      { "b", &buckets_opt}, 
      {NULL, NULL}
    };
    output.uint32_t = 0;
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 &argc, &argv,
				 options, optvals);
 
    buckets = buckets_opt.uint32_t;
    if (buckets < 1) buckets = 1;
    if (buckets > 1) {
	const char * fvargs[] = { "srcip", "dstip", "ipprotocol", "srcip", "dstip" };
        fieldvec.init(dts, 5, fvargs);
    }

    strucio.resize(buckets);
    for (int i = 0; i < buckets; ++i) {
	strucio[i].setPcap(this);

    	if (rotate_size.uint32_t) {
      		strucio[i].set_rotate_size(rotate_size.uint32_t * 1024 * 1024);
    	} else if (rotate_time_opt.uint32_t) {
                rotate_time = rotate_time_opt.uint32_t;
      		strucio[i].set_rotate_time(rotate_time);
    	}

	std::string fname(output.string_t);
	if (buckets > 1) fname += str(boost::format(".%d") % i);
	strucio[i].register_file(fname.c_str());

    	strucio[i].set_use_gzip(gzip.boolean_t);
    }
  }

  snaplen_type = dts->requiretype("int");
  linktype_type = dts->requiretype("int");

  snaplen_field = dts->requirefield("snaplen");
  linktype_field = dts->requirefield("linktype");
  ts_field = dts->requirefield("ts");
  uint32_field = dts->requirefield("uint32");

  pcap_file_header.magic = TCPDUMP_MAGIC;
  pcap_file_header.version_major = 2;
  pcap_file_header.version_minor = 4;
  pcap_file_header.thiszone = 0;
  pcap_file_header.sigfigs = 0;

  dts_pkthdr_type = dts->requiretype("packet");
  assert(dts_pkthdr_type);
}

