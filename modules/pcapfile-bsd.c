/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1997
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * savefile.c - supports offline use of tcpdump
 *      Extraction/creation by Jeffrey Mogul, DECWRL
 *      Modified by Steve McCanne, LBL.
 *
 * Used to save the received packet headers, after filtering, to
 * a file, and then read them later.
 * The first record in the file contains saved values for the machine
 * dependent values so we can print the dump file on any architecture.
 */

#include "pcapfile.h"
#include <pcap.h>

#define TCPDUMP_MAGIC 0xa1b2c3d4
#define TCPDUMP_MAGIC_LINUX 0xa1b2cd34

/* Stuff stolen from tcpdump.org savefile.c (BSD license) */

static void swap_hdr(struct pcap_file_header *hp)
{
        hp->version_major = SWAPSHORT(hp->version_major);
        hp->version_minor = SWAPSHORT(hp->version_minor);
        hp->thiszone = SWAPLONG(hp->thiszone);
        hp->sigfigs = SWAPLONG(hp->sigfigs);
        hp->snaplen = SWAPLONG(hp->snaplen);
        hp->linktype = SWAPLONG(hp->linktype);
}

void parse_pcapfile(struct state * state, struct pcap_file_header * hdr) {
  if (hdr->magic != TCPDUMP_MAGIC) {
    state->swapped = 1;

    if (SWAPLONG(hdr->magic) != TCPDUMP_MAGIC) {
      state->swapped = 0;
      state->extended = 1;

      if (hdr->magic != TCPDUMP_MAGIC_LINUX) {
	if (SWAPLONG(hdr->magic) != TCPDUMP_MAGIC_LINUX) {
	  fprintf(stderr, "bad dump file format");
	  exit(-1);
	}
	state->swapped = 1;
      }
    }
  }
  if (state->swapped)
    swap_hdr(hdr);

  if (hdr->version_major < PCAP_VERSION_MAJOR) {
    fprintf(stderr, "archaic file format");
    exit(-1);
  }
}


void fixup_pcap(struct state * state, struct old_pcap_pkthdr * hdr) {
  if (state->swapped) {
    hdr->caplen = SWAPLONG(hdr->caplen);
    hdr->len = SWAPLONG(hdr->len);
    hdr->ts.tv_sec = SWAPLONG(hdr->ts.tv_sec);
    hdr->ts.tv_usec = SWAPLONG(hdr->ts.tv_usec);
  }

  /*
   * We interchanged the caplen and len fields at version 2.3,
   * in order to match the bpf header layout.  But unfortunately
   * some files were written with version 2.3 in their headers
   * but without the interchanged fields.
   */
  if (state->pcap_file_header.version_minor < 3 ||
      (state->pcap_file_header.version_minor == 3 && hdr->caplen > hdr->len)) {
    int t = hdr->caplen;
    hdr->caplen = hdr->len;
    hdr->len = t;
  }
}


