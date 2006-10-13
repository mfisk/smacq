#! /bin/sh

TESTFILE=0325@1112-snort.pcap
PYTHONPATH=$TEST; export PYTHONPATH

$SMACQQ -f - <<EOF 
pcapfile $TESTFILE | python("pythontest.Dumper", "foobar") | print ts
EOF

exit 1
