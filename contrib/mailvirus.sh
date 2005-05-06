#!/bin/sh

#
# This one uses a window of a fixed number of records
#
sed 's/  */ /g' "$@" | smacqq -g -q "\
	print a.month a.day a.time a.6 a.11 b.14 b.15 \
	from (expr('count + 150') from count(-a) from tabularinput(-d ' ' month day time host logger msgid)) \
		a until (a.expr < new.a.count), \
		b until (b.expr < new.b.count) \
	where a.msgid=b.msgid and substr(a.10,'X-PerlMx-Virus-Detected:') and substr(b.7, 'from=') "

exit

#
# This one uses a time-based window, but time parsing is VERY slow
#
sed 's/  */ /g' "$@" | smacqq -g -q  "\
	print a.time.ctime a.6 a.11 b.14 b.15 \
	from tabularinput(-d ' ' month day time:timeval host logger msgid) \
		a until (a.time.double + 10 < new.b.time.double), \
		b until (b.time.double + 10 < new.b.time.double) \
	where a.msgid=b.msgid and substr(a.10,'X-PerlMx-Virus-Detected:') and substr(b.7, 'from=') "
