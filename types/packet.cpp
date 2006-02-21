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
#include <dts-module.h>
#include <dts_packet.h>

// XXX: This implementation assumes ethernet

/*
* Support routines
*/

#define field_offset(s,f) (long int)(&(((struct s *)0)->f))

static inline struct dts_pkthdr * get_pcap(DtsObject datum) {
	return (struct dts_pkthdr*)datum->getdata();
}

static inline struct ether_header * get_ether(DtsObject datum) {
	return (struct ether_header*)(get_pcap(datum)+1);
}

static inline struct ip * get_ip(DtsObject datum) {
	struct ether_header * ethhdr = get_ether(datum);

	if (ethhdr->ether_type != htons(ETHERTYPE_IP)) return NULL; // Not IP
	else return (struct ip*)(ethhdr+1);
}

static inline struct tcphdr * get_tcp(DtsObject datum) {
	struct ip * iphdr = get_ip(datum);
	if (!iphdr) return NULL;
	if (iphdr->ip_p != 6) return NULL;
	
	return (struct tcphdr *)((char *)iphdr + (iphdr->ip_hl<<2));
}

static inline struct icmp * get_icmp(DtsObject datum) {
	struct ip * iphdr = get_ip(datum);
	if (!iphdr) return NULL;
	if (iphdr->ip_p != 1) return NULL;
	
	return (struct icmp *)((char *)iphdr + (iphdr->ip_hl<<2));
}

static inline struct udphdr * get_udp(DtsObject datum) {
	struct ip * iphdr = get_ip(datum);
	if (!iphdr) return NULL;
	if (iphdr->ip_p != 17) return NULL;
	
	return (struct udphdr *)((char *)iphdr + (iphdr->ip_hl<<2));
}

static inline int gettcpfield(DtsObject datum, DtsObject data, int offset) {
	struct tcphdr * tcphdr = get_tcp(datum);
	if (!tcphdr) return 0;
	
	data->setdata(((char*)tcphdr)+offset);
	return 1;
}

static inline int getudpfield(DtsObject datum, DtsObject data, int offset) {
	struct udphdr * udphdr = get_udp(datum);
	if (!udphdr) return 0;
	
	data->setdata(((char*)udphdr)+offset);
	return 1;
}

static inline int getipfield(DtsObject datum, DtsObject data, int offset) {
	struct ip * iphdr = get_ip(datum);
	if (!iphdr) return 0;
	
	data->setdata(((char*)iphdr)+offset);
	return 1;
}

/*
* Methods
*/
int dts_pkthdr_get_packet(DtsObject datum, DtsObject data) {
	data->setdata(datum->getdata() + sizeof(struct dts_pkthdr));
	data->setsize(datum->getsize() - sizeof(struct dts_pkthdr));
	//fprintf(stderr, "packet is %d bytes vs. caplen of %d bytes\n", data->getsize(), get_pcap(datum)->pcap_pkthdr.caplen);

	return 1;
}

int dts_pkthdr_get_payload(DtsObject datum, DtsObject data) {
	//struct ip * iphdr = get_ip(datum);
	struct tcphdr * tcphdr = get_tcp(datum);

	if (tcphdr) {
		data->setdata(((char*)tcphdr) + tcphdr->th_off * 4);
		data->setsize(datum->getsize() + (unsigned long)datum->getdata() - (unsigned long)data->getdata());
		//fprintf(stderr, "payload is %d bytes of %d at %p of %p\n", data->getsize(), datum->getsize(), data->getdata(), datum->getdata());
		return 1;
	}

	struct udphdr * udphdr = get_udp(datum);

	if (udphdr) {
		data->setdata(udphdr+1);
		data->setsize(datum->getsize() + (unsigned long)datum->getdata() - (unsigned long)data->getdata());
		return 1;
	}

	// Give up decoding (XXX: Handle ICMP echo)
	return 0;
}

int dts_pkthdr_get_dstport(DtsObject datum, DtsObject data) {
	if (!gettcpfield(datum, data, field_offset(tcphdr, th_dport))) 
		if (!getudpfield(datum, data, field_offset(udphdr, uh_dport))) 
			return 0;

	return 1;
}

int dts_pkthdr_get_srcport(DtsObject datum, DtsObject data) {
	if (!gettcpfield(datum, data, field_offset(tcphdr, th_sport))) 
		if (!getudpfield(datum, data, field_offset(udphdr, uh_sport)))
			return 0;

	return 1;
}

int dts_pkthdr_get_seq(DtsObject datum, DtsObject data) {
	return gettcpfield(datum, data, field_offset(tcphdr, th_seq));
}

int dts_pkthdr_get_ack(DtsObject datum, DtsObject data) {
	return gettcpfield(datum, data, field_offset(tcphdr, th_ack));
}

int dts_pkthdr_get_dstip(DtsObject datum, DtsObject data) {
	return getipfield(datum, data, field_offset(ip, ip_dst));
}

int dts_pkthdr_get_srcip(DtsObject datum, DtsObject data) {
	return getipfield(datum, data, field_offset(ip, ip_src));
}

int dts_pkthdr_get_ipid(DtsObject datum, DtsObject data) {
	return getipfield(datum, data, field_offset(ip, ip_id));
}

int dts_pkthdr_get_ttl(DtsObject datum, DtsObject data) {
	return getipfield(datum, data, field_offset(ip, ip_ttl));
}

int dts_pkthdr_get_srcmac(DtsObject datum, DtsObject data) {
	struct ether_header * ethhdr = get_ether(datum);
	data->setdata(&(ethhdr->ether_shost));
	return 1;
}

int dts_pkthdr_get_dstmac(DtsObject datum, DtsObject data) {
	struct ether_header * ethhdr = get_ether(datum);
	data->setdata(&(ethhdr->ether_dhost));
	return 1;
}

int dts_pkthdr_get_protocol(DtsObject datum, DtsObject data) {
	return getipfield(datum, data, field_offset(ip, ip_p));
}
		  
int dts_pkthdr_get_ts(DtsObject datum, DtsObject data) {
	data->setdata(&(get_pcap(datum)->pcap_pkthdr.ts));
	return 1;
}

static int ZERO = 0;
static int ONE = 1;

int dts_pkthdr_get_urg(DtsObject datum, DtsObject data) {
	struct tcphdr * tcphdr = get_tcp(datum);
	if (!tcphdr) return 0;

	data->setdata(((tcphdr->th_flags & TH_URG) ? &ONE : &ZERO));
	return 1;
}

int dts_pkthdr_get_tcpwindow(DtsObject datum, DtsObject data) {
	struct tcphdr * tcphdr = get_tcp(datum);
	if (!tcphdr) return 0;
	
	dts_data_as(data, unsigned short) = tcphdr->th_win;
	return 1;
}

int dts_pkthdr_get_urgptr(DtsObject datum, DtsObject data) {
	return gettcpfield(datum, data, field_offset(tcphdr, th_urp));
}

int dts_pkthdr_get_icmp_type(DtsObject datum, DtsObject data) {
	struct icmp * icmphdr = get_icmp(datum);
	if (!icmphdr) return 0;
	
	data->setdata(&icmphdr->icmp_type);
	return 1;
}

int dts_packet_get_wtap_field(DtsObject datum, DtsObject wtapo, dts_field_element field) {
	return 0;
}

struct dts_field_spec dts_type_packet_fields[] = {
/*
{ "int",	"linktype",	NULL },
{ "int",	"snaplen",	NULL },  
*/

{ "timeval",	"ts",		NULL },
{ "uint32",	"caplen",	NULL },
{ "uint32",	"len",		NULL },

/*
{ "int",	"ifindex",	NULL },
{ "ushort",	"ethertype",	NULL },
{ "ubyte",	"pkt_type",	NULL },

{ "ubyte",	"padding",	NULL },
*/

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
{ "nushort",	"tcpwindow",    dts_pkthdr_get_tcpwindow },

{ "bytes",	"payload",    dts_pkthdr_get_payload },

{ "ubyte",	"icmptype",    dts_pkthdr_get_icmp_type },

{ "timeval",	"timeseries",    dts_pkthdr_get_ts },

{ NULL,		NULL,		NULL }
};


#ifndef WITH_ETHEREAL
struct dts_type_info dts_type_packet_table = {
  size: -1
};
#else

#include "packet_epan.cpp"

struct dts_type_info dts_type_packet_table = {
  size: -1, 
  fromstring: NULL,
  lt: NULL,
  getfield: epan_getfield
};

#endif /* WITH_ETHEREAL */
