#include <sys/types.h>
#ifdef linux
#define __FAVOR_BSD
#else
#include <netinet/in_systm.h>
#endif
#include <net/ethernet.h>
#include <netinet/in.h>
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

static inline struct ip * get_ip(const dts_object * datum) {
  struct ether_header * ethhdr = (struct ether_header*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);
  
  if (ethhdr->ether_type != htons(ETHERTYPE_IP)) return NULL; // Not IP
  else return (struct ip*)(ethhdr+1);
}

static inline struct tcphdr * get_tcp(const dts_object * datum) {
  struct ip * iphdr = get_ip(datum);
  if (!iphdr) return NULL;
  if (iphdr->ip_p != 6) return NULL;

  return (struct tcphdr *)((char *)iphdr + (iphdr->ip_hl<<2));
}

static inline struct icmp * get_icmp(const dts_object * datum) {
  struct ip * iphdr = get_ip(datum);
  if (!iphdr) return NULL;
  if (iphdr->ip_p != 1) return NULL;

  return (struct icmp *)((char *)iphdr + (iphdr->ip_hl<<2));
}

static inline struct udphdr * get_udp(const dts_object * datum) {
  struct ip * iphdr = get_ip(datum);
  if (!iphdr) return NULL;
  if (iphdr->ip_p != 17) return NULL;

  return (struct udphdr *)((char *)iphdr + (iphdr->ip_hl<<2));
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
  struct ip * iphdr = get_ip(datum);
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
  if (!gettcpfield(datum, data, field_offset(tcphdr, th_dport))) 
    if (!getudpfield(datum, data, field_offset(udphdr, uh_dport))) 
      return 0;
 
  return 1;
}

int dts_pkthdr_get_srcport(const dts_object * datum, void ** data, int * len) {
  if (!gettcpfield(datum, data, field_offset(tcphdr, th_sport))) 
    if (!getudpfield(datum, data, field_offset(udphdr, uh_sport)))
      return 0;
 
  return 1;
}

int dts_pkthdr_get_seq(const dts_object * datum, void ** data, int * len) {
  *len = 4;
  return gettcpfield(datum, data, field_offset(tcphdr, th_seq));
}

int dts_pkthdr_get_dstip(const dts_object * datum, void ** data, int * len) {
  *len = sizeof(struct in_addr);
  return getipfield(datum, data, field_offset(ip, ip_dst));
}

int dts_pkthdr_get_srcip(const dts_object * datum, void ** data, int * len) {
  *len = sizeof(struct in_addr);
  return getipfield(datum, data, field_offset(ip, ip_src));
}

int dts_pkthdr_get_ipid(const dts_object * datum, void ** data, int * len) {
  *len = 2;
  return getipfield(datum, data, field_offset(ip, ip_id));
}

int dts_pkthdr_get_ttl(const dts_object * datum, void ** data, int * len) {
  *len = 1;
  return getipfield(datum, data, field_offset(ip, ip_ttl));
}

int dts_pkthdr_get_srcmac(const dts_object * datum, void ** data, int * len) {
  struct ether_header * ethhdr = (struct ether_header*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);

  *len = 6;
  *data = &(ethhdr->ether_shost);
  return 1;
}

int dts_pkthdr_get_dstmac(const dts_object * datum, void ** data, int * len) {
  struct ether_header * ethhdr = (struct ether_header*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);

  *len = 6;
  *data = &(ethhdr->ether_dhost);
  return 1;
}

int dts_pkthdr_get_protocol(const dts_object * datum, void ** data, int * len) {
  *len = sizeof(guint16);
  return getipfield(datum, data, field_offset(ip, ip_p));
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

  *data = ((tcphdr->th_flags & TH_URG) ? &ONE : &ZERO);
  *len = 1;
  return 1;
}

int dts_pkthdr_get_urgptr(const dts_object * datum, void ** data, int * len) {
  return gettcpfield(datum, data, field_offset(tcphdr, th_urp));
}

int dts_pkthdr_get_icmp_type(const dts_object * datum, void ** data, int * len) {
  struct icmp * icmphdr = get_icmp(datum);
  if (!icmphdr) return 0;

  *data = &icmphdr->icmp_type;
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
