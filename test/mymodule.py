#! /usr/bin/env python

class Censor:
    def __init__(self, *args):
        print ('init', args)

    def __call__(self, dts):
        try:
            print ('call',
                   dts['srcip'].uint,
                   dts['dstip'].uint,
                   dts['ipprotocol'].uint,
                   dts['len'].uint)
        except KeyError:
            print ('call', 'KeyError')

init = Censor
