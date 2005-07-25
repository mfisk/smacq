#ifndef FLOWTYPE_PACKET_H
#define FLOWTYPE_PACKET_H

#include <stdint.h>

// Always 32-bit timevals, even on 64-mit machines.
struct timeval_32 {
	uint32_t tv_sec;
	uint32_t tv_usec;
};

#include <pcap.h>
#include <smacq.h>

/*
 * Each packet in the dump file is prepended with this generic header.
 * This gets around the problem of different headers for different
 * packet interfaces.
 */
struct old_pcap_pkthdr {
        struct timeval_32 ts;      /* time stamp */
        bpf_u_int32 caplen;     /* length of portion present */
        bpf_u_int32 len;        /* length this packet (off wire) */
};

struct extended_pkthdr {
	/* Stuff added by the Linux wackos */
        int ifindex;
        unsigned short protocol;
        unsigned char pkt_type;
};

struct dts_pkthdr {
/*
  int linktype;
  int snaplen;
*/
  struct old_pcap_pkthdr pcap_pkthdr;
/*
  struct extended_pkthdr extended;
*/
};


#endif
