TESTS = arith.test bag.test bagfirst.test bagtail.test dfa.test dynamic.test expr.test flowid.test groupby.test groupby2.test having.test head.test iplookup.test last.test mask.test multistr.test or.test orand.test orfunc.test pipe.test subfield.test uniq.test unixq.test @OPTIONAL_TESTS@
EXTRA_TESTS = sqlinsert.test

#Disabled tests: pcapfile.test (false alarm on big-endian platforms)

TESTS_ENVIRONMENT = SRCDIR=$(srcdir) TOP_BUILDDIR=$(top_builddir) $(top_srcdir)/misc/runtest
