#include "dts_packet.h"

#if 1
#include <zlib.h>
#else
#define gzFile FILE
#define gzopen fopen
#define gzdopen fdopen
#define gzread(file,buf,size) fread(buf,1,size,file)
#define gzwrite(file,buf,size) fwrite(buf,1,size,file)
#define gzclose fclose
#endif


struct pcapfile_options {
  char * output;
};

struct state {
	/* State if using libpcap */
  pcap_t * pcap;		
  dts_object * datum;	
  pcap_dumper_t * dumper;

	/* Dynamic dataflow environment */
  smacq_environment * env;
  int argc;
  char ** argv;
  int 	dts_pkthdr_type;		

  int produce;			/* Does this instance produce */
  gzFile * gzfile;
  struct pcap_file_header pcap_file_header;
  int swapped;
  int extended;
  long long outputleft;
  unsigned long long maxfilesize;
  int suffix;

  struct pcapfile_options opts;
};

extern void parse_pcapfile(struct state * state, struct pcap_file_header * hdr);
void fixup_pcap(struct state * state, struct old_pcap_pkthdr * hdr);

#define SWAPLONG(y) \
((((y)&0xff)<<24) | (((y)&0xff00)<<8) | (((y)&0xff0000)>>8) | (((y)>>24)&0xff))
#define SWAPSHORT(y) \
        ( (((y)&0xff)<<8) | ((u_short)((y)&0xff00)>>8) )
