TESTS = arith.test bag.test bagfirst.test bagfirstdifferent.test bagtail.test dfa.test dynamic.test expr.test flowid.test groupby.test groupby2.test having.test head.test iplookup.test last.test.sh mask.test multistr.test or.test orand.test orfunc.test pipe.test subfield.test uniq.test unixq.test join.test joinuntil.test interleave.test printboulder.test times.test maillogjoin.test countobj.test deskew.test sort.test define.test dominant.test @OPTIONAL_TESTS@
EXTRA_TESTS = python.test.sh sqlite.test.sh

#Disabled tests: pcapfile.test (false alarm on big-endian platforms)

TESTS_ENVIRONMENT = OUTDIR=`pwd` SRCDIR=$(srcdir) TOP_SRCDIR=`cd $(top_srcdir); pwd` BINDIR=`cd $(top_builddir); pwd`/bin/ $(top_srcdir)/misc/runtest
