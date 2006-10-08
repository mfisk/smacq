#! /bin/sh

PYTHONPATH=$OUTDIR/../pysmacq/.libs:$SRCDIR/../pysmacq:$TEST
export PYTHONPATH

TESTFILE=0325@1112-snort.pcap
export TESTFILE

python2.3 <<EOF
import smacq
print "Imported successfully"

pcapfile = '0325@1112-snort.pcap'

q = smacq.SmacqQuery('pcapfile(%s)' % (pcapfile))
q >>= 'where srcport > 137'

print q

for d in q:
	print d['srcip'], d['dstip'], d['srcport'], d['dstport'], d.has_key('foo')

EOF

