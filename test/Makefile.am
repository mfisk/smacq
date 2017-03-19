TESTS = strings.test arith.test bag.test bagfirst.test bagfirstdifferent.test bagtail.test dfa.test dynamic.test expr.test flowid.test groupby.test groupby2.test having.test head.test iplookup.test last.test.sh mask.test multistr.test or.test orand.test orfunc.test pipe.test subfield.test uniq.test unixq.test join.test joinuntil.test interleave.test printboulder.test times.test maillogjoin.test countobj.test changes.test deskew.test sort.test define.test dominant.test @OPTIONAL_TESTS@

EXTRA_DIST = 0325@1112-snort.pcap arith.test arith.test.out bagfirstdifferent.test bagfirstdifferent.test.out bagfirst.test bagfirst.test.out bagtail.test bagtail.test.out bag.test bag.test.out changes.test changes.test.out countobj.test countobj.test.out CVS define.test define.test.out deskew.test deskew.test.out dfa.test dfa.test.dfa dfa.test.out dominant.test dominant.test.out dynamic.test dynamic.test.out expr.test expr.test.out flowid.test flowid.test.out groupby2.test groupby2.test.out groupby.test groupby.test.out having.test having.test.out head.test head.test.out interleave.test interleave.test.out iplookup.test iplookup.test.out join.test join.test.out joinuntil.test joinuntil.test.out last.test.sh last.test.sh.out maillogjoin.data maillogjoin.test maillogjoin.test.out Makefile.am Makefile.in mask.test mask.test.out multistr.test multistr.test.out orand.test orand.test.out orfunc.test orfunc.test.out or.test or.test.out pcapfile.test pcapfile.test.out pipe.test pipe.test.out printboulder.test printboulder.test.out pysmacq.test.sh.in pysmacq.test.sh.out pythontest.py pythontest.pyc python.test.sh python.test.sh.out sort.test sort.test.out sqlite.test.sh sqlite.test.sh.out strings.test strings.test.out subfield.test subfield.test.out times.test times.test.out uniq.test uniq.test.out unixq.test unixq.test.out

EXTRA_TESTS = python.test.sh pysmacq.test.sh sqlite.test.sh
CLEANFILES = *.err *.run *.diff

#Disabled tests: pcapfile.test (false alarm on big-endian platforms)

AUTOMAKE_OPTIONS=serial-tests
TESTS_ENVIRONMENT = OUTDIR=`pwd` SRCDIR=$(srcdir) TOP_SRCDIR=`cd $(top_srcdir); pwd` BINDIR=`cd $(top_builddir); pwd`/bin/ $(top_srcdir)/misc/runtest
