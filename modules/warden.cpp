#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SmacqModule.h>
#include <sys/types.h>
#ifndef linux 
#include <netinet/in_systm.h>
struct tcphdr
  {
    u_int16_t source;
    u_int16_t dest;
    u_int32_t seq;
    u_int32_t ack_seq;
#  if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int16_t res1:4;
    u_int16_t doff:4;
    u_int16_t fin:1;
    u_int16_t syn:1;
    u_int16_t rst:1;
    u_int16_t psh:1;
    u_int16_t ack:1;
    u_int16_t urg:1;
    u_int16_t res2:2;
#  elif __BYTE_ORDER == __BIG_ENDIAN
    u_int16_t doff:4;
    u_int16_t res1:4;
    u_int16_t res2:2;
    u_int16_t urg:1;
    u_int16_t ack:1;
    u_int16_t psh:1;
    u_int16_t rst:1;
    u_int16_t syn:1;
    u_int16_t fin:1;
#  else
#   error "Adjust your <bits/endian.h> defines"
#  endif
    u_int16_t window;
    u_int16_t check;
    u_int16_t urg_ptr;
};
#endif
#include <netinet/in.h>
#include <netinet/ip.h>
#ifdef linux
#include <netinet/tcp.h>
#endif
#include <net/ethernet.h>
#include <dts_packet.h>

static struct smacq_options options[] = {
  END_SMACQ_OPTIONS
};

SMACQ_MODULE(warden, 
  PROTO_CTOR(warden);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  DtsObject datum;
); 

static inline int min(int a, int b) {
  return (a<b ? a : b);
}

wardenModule::wardenModule(smacq_init * context) : SmacqModule(context) {}

smacq_result wardenModule::consume(DtsObject datum, int & outchan) {
  struct dts_pkthdr * pkthdr;
  struct ether_header * ethhdr;
  struct ip * iphdr;
  struct tcphdr * tcphdr;

 datum = datum->make_writable();
 pkthdr = (struct dts_pkthdr*)datum->getdata();
 ethhdr = (struct ether_header*)(pkthdr+1);
 if (ethhdr->ether_type != htons(ETHERTYPE_IP)) return SMACQ_PASS; // Not IP
 else iphdr = (struct ip *)(ethhdr+1);

 if (iphdr->ip_p != 6) return SMACQ_PASS;
 else tcphdr = (struct tcphdr *)((char *)iphdr + (iphdr->ip_hl<<2));
 
 // Urgent ptr without urgent bit
 if (tcphdr->urg) {
   tcphdr->urg_ptr = 0;
 }

 // Reserved bits
 tcphdr->res1 = 0;
 tcphdr->res2 = 0;

 // RST implies no TCP payload
 if (tcphdr->rst) {
   // Make sure IP payload size = TCP header only
   iphdr->ip_len = htons((iphdr->ip_hl<<2) + (tcphdr->doff << 2));
 }

 // Urgent pointer must be within TCP payload
 if (tcphdr->urg_ptr > (ntohs(iphdr->ip_len) - (tcphdr->doff<<2))) {
   tcphdr->urg_ptr = 0;
   tcphdr->urg = 0;
}

 // Remove covert trailers in the payload
 pkthdr->pcap_pkthdr.caplen = pkthdr->pcap_pkthdr.len = ntohs(iphdr->ip_len) + sizeof(struct ether_header);

 return (smacq_result)(SMACQ_FREE|SMACQ_PRODUCE);
}

smacq_result wardenModule::produce(DtsObject & datump, int & outchan) {
  if (datum) {
    datump = datum;
    datum = NULL;
    return SMACQ_PASS;
  } else {
    return SMACQ_END;
  }
}

