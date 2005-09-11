#!/bin/sh 
DB=`pwd`/sqlitedb

rm -f $DB

#sqlite3 $DB << EOM
#create table foo(ts varchar(80), srcip varchar(32), dstip varchar(32));
#EOM

$SMACQQ -m -g <<EOF 
sqlinsert -t foo -d "URI=$DB" ts srcip dstip from (uniq srcip, dstip from pcapfile(0325@1112-snort.pcap))
EOF

$SMACQQ -m -g <<EOF 
print -v ts srcip dstip from sqlquery(-d "URI=$DB" foo)
EOF

