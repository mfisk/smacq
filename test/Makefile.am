TESTS = arith.test bag.test bagfirst.test bagtail.test dfa.test dynamic.test expr.test flowid.test groupby.test groupby2.test having.test head.test iplookup.test last.test mask.test multistr.test or.test orand.test orfunc.test pipe.test subfield.test uniq.test unixq.test join.test joinuntil.test interleave.test printboulder.test @PYTHON_TESTS@ @OPTIONAL_TESTS@
EXTRA_TESTS = python.test sqlinsert.test

#Disabled tests: pcapfile.test (false alarm on big-endian platforms)

TESTS_ENVIRONMENT = SRCDIR=$(srcdir) TOP_SRCDIR=$(top_srcdir) BINDIR=$(top_builddir)/bin/ $(top_srcdir)/misc/runtest
