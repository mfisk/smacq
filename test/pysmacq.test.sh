#! /bin/sh

PYTHONPATH=$OUTDIR/../pysmacq/.libs:$SRCDIR/../pysmacq:$TEST
export PYTHONPATH

TESTFILE=0325@1112-snort.pcap
export TESTFILE

python2.3 <<EOF
import smacq
print "Imported successfully"

pcapfile = '0325@1112-snort.pcap'
query = 'print from pcapfile(%s)' % (pcapfile)

q1 = 'print srcip dstip srcport dstport' 
q2 = 'pcapfile(%s)' % (pcapfile)

g2 = smacq.SmacqQuery(q2)
g1 = smacq.SmacqQuery(q1)

print g1
print g2

g2 += g1

print g2

g2.run()

g2.busy_loop()
EOF

