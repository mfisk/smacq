#include <stdio.h>
#include <StrucioStream.h>
#include <SmacqModule.h>

#define ZBUFSIZE 4096
#define RECSIZE sizeof(struct fts3rec_v7)

#define FT_TLV_EX_VER             0x2     /* uint16_t : export version */
#define FT_TLV_HEADER_FLAGS       0x8     /* uint32_t : FT_HEADER_FLAG_* */

#define FT_HEADER_FLAG_COMPRESS     0x2    /* compression enabled */
#define FT_HEADER_LITTLE_ENDIAN     1      /* stream data is little endian */
#define FT_HEADER_BIG_ENDIAN        2      /* stream data is big endian */


struct ftheader_gen {
  uint8_t  magic1;                 /* 0xCF */
  uint8_t  magic2;                 /* 0x10 (cisco flow) */
  uint8_t  byte_order;             /* 1 for little endian (VAX) */
                                  /* 2 for big endian (Motorolla) */
  uint8_t  s_version;              /* flow stream format version 1 or 3 */
  uint32_t head_off;               /* offset from beginning of file to first record */ 
};

struct fttlv {
  uint16_t t, l;         /* type, length */
};

struct fts3rec_v7 {
  uint32_t unix_secs;      /* Current seconds since 0000 UTC 1970 */
  uint32_t unix_nsecs;     /* Residual nanoseconds since 0000 UTC 1970 */
  uint32_t sysUpTime;      /* Current time in millisecs since router booted */
  uint32_t exaddr;         /* Exporter IP address */
  uint32_t srcaddr;        /* Source IP Address */
  uint32_t dstaddr;        /* Destination IP Address */
  uint32_t nexthop;        /* Next hop router's IP Address */
  uint16_t input;          /* Input interface index */
  uint16_t output;         /* Output interface index */
  uint32_t dPkts;          /* Packets sent in Duration */
  uint32_t dOctets;        /* Octets sent in Duration. */
  uint32_t First;          /* SysUptime at start of flow */
  uint32_t Last;           /* and of last packet of flow */
  uint16_t srcport;        /* TCP/UDP source port number or equivalent */
  uint16_t dstport;        /* TCP/UDP destination port number or equiv */
  uint8_t  prot;           /* IP protocol, e.g., 6=TCP, 17=UDP, ... */
  uint8_t  tos;            /* IP Type-of-Service */
  uint8_t  tcp_flags;      /* OR of TCP header bits */
  uint8_t  flags;          /* Reason flow discarded, etc */
  uint8_t  engine_type;    /* Type of flow switching engine (RP,VIP,etc.) */
  uint8_t  engine_id;      /* Slot number of the flow switching engine */
  uint8_t  src_mask;       /* mask length of source address */
  uint8_t  dst_mask;       /* mask length of destination address */
  uint16_t src_as;         /* AS of source address */
  uint16_t dst_as;         /* AS of destination address */
  uint32_t router_sc;      /* ID of router shortcut by switch */
};


SMACQ_MODULE(ft,
  PROTO_CTOR(ft);
  PROTO_DTOR(ft);
  PROTO_CONSUME();
  PROTO_PRODUCE();

  int netflow7_type;
  StrucioStream * fh;

  bool compressed;
  z_stream zs;
  unsigned char zbuf[ZBUFSIZE];
  unsigned char recbuf[RECSIZE];
);

smacq_result ftModule::produce(DtsObject & datum, int & outchan) {
  if (fh) {
     if (compressed) {
        if (!zs.avail_in) {
            /* Refill input buffer */
            int n = fh->Read(zbuf, ZBUFSIZE);
            if (!n) return SMACQ_FREE;
            zs.avail_in = n;
            zs.next_in = zbuf;
        }
        int err = inflate(&zs, Z_SYNC_FLUSH);

        if ((err != Z_OK) && (err != Z_STREAM_END)) {
            perror("inflate");
            return SMACQ_FREE;
        }

        if (!zs.avail_out) {
            /* Filled-up output structure */
            struct fts3rec_v7 * flow = (struct fts3rec_v7*) recbuf;

            /* Byte-swap IPs for some reason */
            if (G_BYTE_ORDER == LITTLE_ENDIAN) {
                /* we want IPs in network byte order, so swap */
	    	flow->srcaddr = GUINT32_SWAP_LE_BE(flow->srcaddr);
	    	flow->dstaddr = GUINT32_SWAP_LE_BE(flow->dstaddr);
	    	flow->exaddr = GUINT32_SWAP_LE_BE(flow->exaddr);
            }

            /* Compute start and end times as absolutes rather than offsets */
            uint32_t start = flow->unix_secs - ((flow->sysUpTime - flow->First) / 1000);
            uint32_t end = flow->unix_secs - ((flow->sysUpTime - flow->Last) / 1000);
            int32_t startus = flow->unix_nsecs/1000 - ((flow->sysUpTime - flow->First) % 1000) * 1000;
            int32_t endus = flow->unix_nsecs/1000 - ((flow->sysUpTime - flow->Last) % 1000) * 1000;

            if (endus < 0) {
               end -= 1;
               endus += 1000000;
            }
            if (startus < 0) {
               start -= 1;
               startus += 1000000;
            }

            /* We repurpose the unix_time for the end timeval and the First/End time for the start timeval */
            flow->unix_secs = end;
            flow->unix_nsecs = endus;
            flow->First = start;
            flow->Last = startus;

            /* Now use this memory region to construct a smacq object (which uses these absolute times) */
            datum = dts->construct(netflow7_type, recbuf);
            unsigned int size = datum->getsize();
            assert(size == RECSIZE);

            zs.next_out = recbuf;
            zs.avail_out = RECSIZE; 

   	    return SMACQ_PASS|SMACQ_PRODUCE;
        } else {
            return SMACQ_FREE|SMACQ_PRODUCE;
        }
     } else if ((datum = fh->construct(dts, netflow7_type))) {
		return SMACQ_PASS|SMACQ_PRODUCE;
     } else {
		return SMACQ_FREE;
     }
  } else {
	return SMACQ_FREE;
  }
}

smacq_result ftModule::consume(DtsObject fileo, int & outchan) {
  if (fh) delete fh; // Clean-up previous file

  fh = StrucioStream::MagicOpen(fileo);

  if (!fh) {
    return SMACQ_FREE;
  }

  struct ftheader_gen h1;

  if (sizeof(struct ftheader_gen) != fh->Read(&h1, sizeof(struct ftheader_gen))) {
    perror("ftModule");
    exit(-1);
  }

  assert(h1.s_version == 3);
  assert(h1.magic1 == 0xcf);
  assert(h1.magic2 == 0x10);
  unsigned int dynlen = h1.head_off - sizeof(struct ftheader_gen);

  while (dynlen > 0) {
    if (dynlen < 4) {
      // Padding
      char padding[4];
      if (dynlen != fh->Read(padding, dynlen)) {
        perror("ftModule(TLV)");
        exit(-1);
      }
      break;
    }
      
    struct fttlv tlv;
    if (sizeof(struct fttlv) != fh->Read(&tlv, sizeof(struct fttlv))) {
      perror("ftModule(TLV)");
      exit(-1);
    }
    char * val = (char*)alloca(tlv.l);
    if (tlv.l != fh->Read(val, tlv.l)) {
      perror("ftModule(TLV)");
      exit(-1);
    }
    dynlen -= (sizeof(struct fttlv) + tlv.l);

    if (tlv.t == FT_TLV_HEADER_FLAGS) {
      uint32_t flags = *(uint32_t*)val;
      if (flags & FT_HEADER_FLAG_COMPRESS) {
        compressed = 1;
        zs.next_in = zbuf;
        zs.next_out = recbuf;
        zs.avail_in = 0;
        zs.avail_out = RECSIZE;
        int r = inflateInit(&zs);
        assert(r == Z_OK);
      }
    } else if (tlv.t == FT_TLV_EX_VER) {
      int version = *(uint16_t*)val;
      assert(version == 7);
    }
  }

  // Now we're at the flow records
  //dfh = new FileStream<gzFile>(fname, fd, mode);

  // Make sure file is in host byte order
  assert((h1.byte_order == FT_HEADER_BIG_ENDIAN) == (G_BYTE_ORDER == G_BIG_ENDIAN));
  
  //fprintf(stderr, "ft: Opening file %s\n", StrucioStream::Filename(fileo).c_str());

  return SMACQ_FREE|SMACQ_PRODUCE;
}

ftModule::ftModule(struct SmacqModule::smacq_init * context) : SmacqModule(context), fh(NULL), compressed(0) {
 SmacqFileModule(context);

 netflow7_type = dts->requiretype("netflow7");
}

ftModule::~ftModule() {
 if (fh) delete fh;
}
