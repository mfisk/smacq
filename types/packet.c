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
  return 1;
}

int dts_pkthdr_get_tcpwindow(const dts_object * datum, dts_object * data) {
  struct tcphdr * tcphdr = get_tcp(datum);
  if (!tcphdr) return 0;

  dts_data_as(data, unsigned short) = tcphdr->th_win;
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

        { END,	NULL,		NULL }
};


#ifndef WITH_ETHEREAL
struct dts_type_info dts_type_packet_table = {
	size: -1
};
#else
int epan_getfield(const dts_object * packet, dts_object * fieldo, dts_field_element element);

struct dts_type_info dts_type_packet_table = {
	size: -1, 
	getfield: epan_getfield
};

#include <epan/ftypes/ftypes.h>
#include <epan/epan.h>
#include <epan/proto.h>
#include <epan/packet.h>
#include <epan/epan_dissect.h>
#include <epan/timestamp.h>
#include <register.h>
#include <prefs.h>

ts_type timestamp_type = ABSOLUTE;

static int did_epan_init = 0;

int epan_getfield(const dts_object * packet, dts_object * fieldo, dts_field_element element) {
  int len;
  int field_id;
  field_info * finfo;
  GPtrArray	*finfos;
  header_field_info * hfinfo;
  epan_dissect_t * edt;
  frame_data fdata;


  struct dts_pkthdr * dhdr = (struct dts_pkthdr*)dts_getdata(packet);
  struct old_pcap_pkthdr * phdr = &dhdr->pcap_pkthdr;
  char * packet_data = (char*)(dhdr+1);

  memset(&fdata, 0, sizeof(frame_data));

  fdata.pkt_len = phdr->len;
  fdata.cap_len = phdr->caplen;
  fdata.lnk_t = 0; /* getfield */

  if (!did_epan_init) {
    epan_init(".", register_all_protocols, register_all_protocol_handoffs);
    prefs_register_modules();
    init_dissection();
    did_epan_init =1;
  }

  {
    char * dts_name, * p;
    dts_field_element field[2];
    field[0] = element;
    field[1] = 0;
    
    dts_name = strdup(dts_field_getname(packet->tenv, field));
    while ((p = index(dts_name, ':'))) {
      p[0] = '.';
    }
    
    //fprintf(stderr, "Getting proto %s\n", dts_name);

    hfinfo =  proto_registrar_get_byname(dts_name);

    if (!hfinfo) {
      fprintf(stderr, "EPAN does not recognize field %s\n", dts_name);
      return 0;
    }
    field_id = hfinfo->id;
    free(dts_name);
  }

  edt = epan_dissect_new(TRUE, TRUE);
  proto_tree_prime_hfid(edt->tree, field_id);
  epan_dissect_run(edt, NULL, packet_data, &fdata, NULL);

  /* The rest of this could be a subfield of an "epan" type */

  finfos = proto_get_finfo_ptr_array(edt->tree, field_id);
  if (!finfos) {
    fprintf(stderr, "Unexpected error: EPAN field not requested\n");
    return 0;
  }
  
  len = finfos->len;

  if (len == 0) {
    //fprintf(stderr, "packet: EPAN field not present\n");
    return 0;
  }

  if (len > 1) {
    fprintf(stderr, "packet: warning: only returning first of multiple values\n");
  }

  finfo = g_ptr_array_index(finfos, 0);
  assert(finfo);


  switch(hfinfo->type) {
    case FT_STRING:
      fieldo->type = dts_requiretype(packet->tenv, "string");
      fieldo->data = fvalue_get(finfo->value);
      fieldo->len = fvalue_length(finfo->value);
      break;
    
    case FT_DOUBLE:
      fieldo->type = dts_requiretype(packet->tenv, "double");
      dts_data_as(fieldo, double) = fvalue_get_floating(finfo->value);
      fieldo->len = 2;
      break;
    
    case FT_NONE:
      fieldo->type = dts_requiretype(packet->tenv, "empty");
      fieldo->len = 0;
      break;
    
    case FT_BOOLEAN:
    case FT_UINT8:
      fieldo->type = dts_requiretype(packet->tenv, "ubyte");
      dts_data_as(fieldo, unsigned char) = fvalue_get_integer(finfo->value);
      fieldo->len = 1;
      break;
    
  case FT_PROTOCOL:
      fieldo->type = dts_requiretype(packet->tenv, "ubyte");
      dts_data_as(fieldo, unsigned char) = fvalue_get_integer(finfo->value);
      fieldo->len = 1;
      break;

  case FT_ETHER:
      fieldo->type = dts_requiretype(packet->tenv, "macaddr");
      fieldo->data  = fvalue_get(finfo->value);
      fieldo->len = 6;
      break;
    
    case FT_UINT16:
      fieldo->type = dts_requiretype(packet->tenv, "ushort");
      dts_data_as(fieldo, uint16_t) = fvalue_get_integer(finfo->value);
      fieldo->len = 2;
      break;
    
    case FT_UINT32:
      fieldo->type = dts_requiretype(packet->tenv, "uint32");
      dts_data_as(fieldo, uint32_t) = fvalue_get_integer(finfo->value);
      fieldo->len = 4;
      break;
    
    case FT_IPv4:
      fieldo->type = dts_requiretype(packet->tenv, "ip");
      fieldo->data = fvalue_get(finfo->value);
      dts_data_as(fieldo, unsigned long) = htonl(dts_data_as(fieldo, unsigned long));
      fieldo->len = 4;
      break;

  default:
      fprintf(stderr, "packet: warning: unsupported EPAN type %s being converted to 'bytes'\n", ftype_pretty_name(hfinfo->type));
      fieldo->type = dts_requiretype(packet->tenv, "bytes");
      fieldo->data = fvalue_get(finfo->value);
      fieldo->len = fvalue_length(finfo->value);
      break;
  }

  epan_dissect_free(edt);
  //free_data_sources(&fdata);

  return 1;
  
  //proto_tree_free_node();
}
#endif /* WITH_ETHEREAL */
