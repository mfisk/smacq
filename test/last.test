#!/bin/sh

$SMACQQ -g -m <<EOF | sort -n
print ts, srcip, dstip from (last srcip, dstip from pcapfile(0325@1112-snort.pcap))
EOF
