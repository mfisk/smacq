#include <smacq.h>
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

static inline int gettcpfield(const dts_object * datum, dts_object * data, int offset) {
  struct tcphdr * tcphdr = get_tcp(datum);
  if (!tcphdr) return 0;

  data->data = ((char*)tcphdr)+offset;
  return 1;
}

static inline int getudpfield(const dts_object * datum, dts_object * data, int offset) {
  struct udphdr * udphdr = get_udp(datum);
  if (!udphdr) return 0;

  data->data = ((char*)udphdr)+offset;
  return 1;
}

static inline int getipfield(const dts_object * datum, dts_object * data, int offset) {
  struct ip * iphdr = get_ip(datum);
  if (!iphdr) return 0;

  data->data = ((char*)iphdr)+offset;
  return 1;
}

/*
 * Methods
 */
int dts_pkthdr_get_packet(const dts_object * datum, dts_object * data) {
  data->data = (void *)((int)dts_getdata(datum) + sizeof(struct dts_pkthdr));
  data->len = ((struct dts_pkthdr *)dts_getdata(datum))->pcap_pkthdr.caplen;
  return 1;
}

int dts_pkthdr_get_payload(const dts_object * datum, dts_object * data) {
  //struct ip * iphdr = get_ip(datum);
  struct tcphdr * tcphdr = get_tcp(datum);
  struct udphdr * udphdr;

  if (tcphdr) {
  	data->data = ((char*)tcphdr) + tcphdr->th_off * 4;
	data->len = datum->len + datum->data - data->data;
  	return 1;
  }

  udphdr = get_udp(datum);

  if (udphdr) {
	data->data = udphdr+1;
	data->len = datum->len + datum->data - data->data;
	return 1;
  }
  return 0;
}

int dts_pkthdr_get_dstport(const dts_object * datum, dts_object * data) {
  if (!gettcpfield(datum, data, field_offset(tcphdr, th_dport))) 
    if (!getudpfield(datum, data, field_offset(udphdr, uh_dport))) 
      return 0;
 
  return 1;
}

int dts_pkthdr_get_srcport(const dts_object * datum, dts_object * data) {
  if (!gettcpfield(datum, data, field_offset(tcphdr, th_sport))) 
    if (!getudpfield(datum, data, field_offset(udphdr, uh_sport)))
      return 0;
 
  return 1;
}

int dts_pkthdr_get_seq(const dts_object * datum, dts_object * data) {
  data->len = 4;
  return gettcpfield(datum, data, field_offset(tcphdr, th_seq));
}

int dts_pkthdr_get_ack(const dts_object * datum, dts_object * data) {
  data->len = 4;
  return gettcpfield(datum, data, field_offset(tcphdr, th_ack));
}

int dts_pkthdr_get_dstip(const dts_object * datum, dts_object * data) {
  data->len = sizeof(struct in_addr);
  return getipfield(datum, data, field_offset(ip, ip_dst));
}

int dts_pkthdr_get_srcip(const dts_object * datum, dts_object * data) {
  data->len = sizeof(struct in_addr);
  return getipfield(datum, data, field_offset(ip, ip_src));
}

int dts_pkthdr_get_ipid(const dts_object * datum, dts_object * data) {
  data->len = 2;
  return getipfield(datum, data, field_offset(ip, ip_id));
}

int dts_pkthdr_get_ttl(const dts_object * datum, dts_object * data) {
  data->len = 1;
  return getipfield(datum, data, field_offset(ip, ip_ttl));
}

int dts_pkthdr_get_srcmac(const dts_object * datum, dts_object * data) {
  struct ether_header * ethhdr = (struct ether_header*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);

  data->len = 6;
  data->data = &(ethhdr->ether_shost);
  return 1;
}

int dts_pkthdr_get_dstmac(const dts_object * datum, dts_object * data) {
  struct ether_header * ethhdr = (struct ether_header*)(((struct dts_pkthdr *)dts_getdata(datum)) + 1);

  data->len = 6;
  data->data = &(ethhdr->ether_dhost);
  return 1;
}

int dts_pkthdr_get_protocol(const dts_object * datum, dts_object * data) {
  data->len = sizeof(unsigned char);
  return getipfield(datum, data, field_offset(ip, ip_p));
}
			  
int dts_pkthdr_get_ts(const dts_object * datum, dts_object * data) {
  struct dts_pkthdr * p = (struct dts_pkthdr*)dts_getdata(datum);
  data->len = sizeof(struct timeval);
  data->data = &(p->pcap_pkthdr.ts);
  return 1;
}

static int ZERO = 0;
static int ONE = 1;

int dts_pkthdr_get_urg(const dts_object * datum, dts_object * data) {
  struct tcphdr * tcphdr = get_tcp(datum);
  if (!tcphdr) return 0;

  data->data = ((tcphdr->th_flags & TH_URG) ? &ONE : &ZERO);
  data->len = 1;
  return 1;
}

int dts_pkthdr_get_urgptr(const dts_object * datum, dts_object * data) {
  return gettcpfield(datum, data, field_offset(tcphdr, th_urp));
}

int dts_pkthdr_get_icmp_type(const dts_object * datum, dts_object * data) {
  struct icmp * icmphdr = get_icmp(datum);
  if (!icmphdr) return 0;

  data->data = &icmphdr->icmp_type;
  data->len = 1;
  return 1;
}

int dts_packet_get_wtap_field(const dts_object * datum, dts_object * wtapo, dts_field_element field) {
  return 0;
}

struct dts_field_spec dts_type_packet_fields[] = {
	{ "int",	"linktype",	NULL },
	{ "int",	"snaplen",	NULL }, 

	{ "timeval",	"ts",		NULL },
	{ "uint32",	"caplen",	NULL },
	{ "uint32",	"len",		NULL },

	{ "int",	"ifindex",	NULL },
	{ "ushort",	"ethertype",	NULL },
	{ "ubyte",	"pkt_type",	NULL },
	{ "ubyte",	"padding",	NULL },

	{ "ipproto",	"ipprotocol",	dts_pkthdr_get_protocol },
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
	{ "nuint32",	"ack",    dts_pkthdr_get_ack },
	{ "nushort",	"urgptr",    dts_pkthdr_get_urgptr },
	{ "ubyte",	"urg",    dts_pkthdr_get_urg },

	{ "bytes",	"payload",    dts_pkthdr_get_payload },

	{ "ubyte",	"icmptype",    dts_pkthdr_get_icmp_type },

	{ "timeval",	"timeseries",    dts_pkthdr_get_ts },

        { END,	NULL,		NULL }
};


int epan_getfield(const dts_object * packet, dts_object * fieldo, dts_field_element element);

struct dts_type_info dts_type_packet_table = {
	size: -1,
	getfield: epan_getfield
};
