#! /usr/bin/env python

class Censor:
    def __init__(self, *args):
        print ('init', args)

    def __call__(self, dts):
        for i in 'srcip', 'dstip', 'ipprotocol', 'len':
            v = dts[i].value
            print (dts[i].type, type(v), v)

init = Censor
