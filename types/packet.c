#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <assert.h>
#include <dts_packet.h>
#include <smacq.h>

// XXX: This implementation assumes ethernet

/*
 * Support routines
 */

#define field_offset(s,f) ((int)&(((struct s *)0)->f))

static inline struct iphdr * get_ip(const dts_object * datum) {
  struct ethhdr * ethhdr = (struct ethhdr*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);
  
  if (ethhdr->h_proto != htons(ETH_P_IP)) return NULL; // Not IP
  else return (struct iphdr*)(ethhdr+1);
}

static inline struct tcphdr * get_tcp(const dts_object * datum) {
  struct iphdr * iphdr = get_ip(datum);
  if (!iphdr) return NULL;
  if (iphdr->protocol != 6) return NULL;

  return (struct tcphdr *)((char *)iphdr + (iphdr->ihl<<2));
}

static inline struct icmphdr * get_icmp(const dts_object * datum) {
  struct iphdr * iphdr = get_ip(datum);
  if (!iphdr) return NULL;
  if (iphdr->protocol != 1) return NULL;

  return (struct icmphdr *)((char *)iphdr + (iphdr->ihl<<2));
}

static inline struct udphdr * get_udp(const dts_object * datum) {
  struct iphdr * iphdr = get_ip(datum);
  if (!iphdr) return NULL;
  if (iphdr->protocol != 17) return NULL;

  return (struct udphdr *)((char *)iphdr + (iphdr->ihl<<2));
}

static inline int gettcpfield(const dts_object * datum, void ** data, int offset) {
  struct tcphdr * tcphdr = get_tcp(datum);
  if (!tcphdr) return 0;

  *data = ((char*)tcphdr)+offset;
  return 1;
}

static inline int getudpfield(const dts_object * datum, void ** data, int offset) {
  struct udphdr * udphdr = get_udp(datum);
  if (!udphdr) return 0;

  *data = ((char*)udphdr)+offset;
  return 1;
}

static inline int getipfield(const dts_object * datum, void ** data, int offset) {
  struct iphdr * iphdr = get_ip(datum);
  if (!iphdr) return 0;

  *data = ((char*)iphdr)+offset;
  return 1;
}

/*
 * Methods
 */
int dts_pkthdr_get_packet(const dts_object * datum, void ** data, int * len) {
  *data = (void *)((int)dts_getdata(datum) + sizeof(struct dts_pkthdr));
  *len = ((struct dts_pkthdr *)dts_getdata(datum))->pcap_pkthdr.caplen;
  return 1;
}

int dts_pkthdr_get_dstport(const dts_object * datum, void ** data, int * len) {
  if (!gettcpfield(datum, data, field_offset(tcphdr, dest))) 
    if (!getudpfield(datum, data, field_offset(udphdr, dest))) 
      return 0;
 
  return 1;
}

int dts_pkthdr_get_srcport(const dts_object * datum, void ** data, int * len) {
  if (!gettcpfield(datum, data, field_offset(tcphdr, source))) 
    if (!getudpfield(datum, data, field_offset(udphdr, source)))
      return 0;
 
  return 1;
}

int dts_pkthdr_get_seq(const dts_object * datum, void ** data, int * len) {
  *len = 4;
  return gettcpfield(datum, data, field_offset(tcphdr, seq));
}

int dts_pkthdr_get_dstip(const dts_object * datum, void ** data, int * len) {
  *len = sizeof(struct in_addr);
  return getipfield(datum, data, field_offset(iphdr, daddr));
}

int dts_pkthdr_get_srcip(const dts_object * datum, void ** data, int * len) {
  *len = sizeof(struct in_addr);
  return getipfield(datum, data, field_offset(iphdr, saddr));
}

int dts_pkthdr_get_ipid(const dts_object * datum, void ** data, int * len) {
  *len = 2;
  return getipfield(datum, data, field_offset(iphdr, id));
}

int dts_pkthdr_get_ttl(const dts_object * datum, void ** data, int * len) {
  *len = 1;
  return getipfield(datum, data, field_offset(iphdr, ttl));
}

int dts_pkthdr_get_srcmac(const dts_object * datum, void ** data, int * len) {
  struct ethhdr * ethhdr = (struct ethhdr*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);

  *len = 6;
  *data = &(ethhdr->h_source);
  return 1;
}

int dts_pkthdr_get_dstmac(const dts_object * datum, void ** data, int * len) {
  struct ethhdr * ethhdr = (struct ethhdr*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);

  *len = 6;
  *data = &(ethhdr->h_dest);
  return 1;
}

int dts_pkthdr_get_protocol(const dts_object * datum, void ** data, int * len) {
  *len = sizeof(guint16);
  return getipfield(datum, data, field_offset(iphdr, protocol));
}
			  
int dts_pkthdr_get_ts(const dts_object * datum, void ** data, int * len) {
  struct dts_pkthdr * p = (struct dts_pkthdr*)dts_getdata(datum);
  *len = sizeof(struct timeval);
  *data = &(p->pcap_pkthdr.ts);
  return 1;
}

static int ZERO = 0;
static int ONE = 1;

int dts_pkthdr_get_urg(const dts_object * datum, void ** data, int * len) {
  struct tcphdr * tcphdr = get_tcp(datum);
  if (!tcphdr) return 0;

  *data = (tcphdr->urg ? &ONE : &ZERO);
  *len = 1;
  return 1;
}

int dts_pkthdr_get_urgptr(const dts_object * datum, void ** data, int * len) {
  return gettcpfield(datum, data, field_offset(tcphdr, urg_ptr));
}

int dts_pkthdr_get_icmp_type(const dts_object * datum, void ** data, int * len) {
  struct icmphdr * icmphdr = get_icmp(datum);
  if (!icmphdr) return 0;

  *data = &icmphdr->type;
  *len = 1;
  return 1;
}


struct dts_field_descriptor dts_type_packet_fields[] = {
	{ "int",	"linktype",	NULL },
	{ "int",	"snaplen",	NULL }, 

	{ "int",	"ifindex",	NULL },
	{ "ushort",	"protocol",	NULL },
	{ "ubyte",	"pkt_type",	NULL },
	{ "ubyte",	"padding",	NULL },

	{ "timeval",	"ts",		NULL },
	{ "uint32",	"caplen",	NULL },
	{ "uint32",	"len",		NULL },

	{ "bytes",	"packet",	dts_pkthdr_get_packet },

	{ "macaddr",	"srcmac",	dts_pkthdr_get_srcmac },
	{ "macaddr",	"dstmac",	dts_pkthdr_get_dstmac },

	{ "ip",	        "srcip",	dts_pkthdr_get_srcip },
	{ "ip",	        "dstip",	dts_pkthdr_get_dstip },

	{ "nushort",	"ipid",     dts_pkthdr_get_ipid },
	{ "ubyte",	"ttl",     dts_pkthdr_get_ttl },

	{ "nushort",	"srcport",    dts_pkthdr_get_srcport },
	{ "nushort",	"dstport",    dts_pkthdr_get_dstport },
	{ "nuint32",	"seq",    dts_pkthdr_get_seq },
	{ "nushort",	"urgptr",    dts_pkthdr_get_urgptr },
	{ "ubyte",	"urg",    dts_pkthdr_get_urg },

	{ "ubyte",	"icmptype",    dts_pkthdr_get_icmp_type },

	{ "timeval",	"timeseries",    dts_pkthdr_get_ts },

        { (char *)END,	NULL,		NULL }
};
