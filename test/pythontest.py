#! /usr/bin/env python

class Dumper:
    def __init__(self, smacq, *args):
        print ('init', args)
        self.smacq = smacq
        self.buf = []

    def __del__(self):
        print ('del', len(self.buf))

    def consume(self, datum):
        for i in 'srcip', 'dstip', 'ipprotocol', 'len':
            v = datum[i].value
            print (i, datum[i].type, type(v), v)
        self.buf.append(datum)
        if len(self.buf) == 5:
            self.smacq.enqueue(datum)
            self.buf = []
