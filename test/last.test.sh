#!/bin/sh

$SMACQQ -g -f - <<EOF | sort -n
print ts, srcip, dstip from (last srcip, dstip from pcapfile(0325@1112-snort.pcap))
EOF
