#! /usr/bin/env python

class Censor:
    def __init__(self, *args):
        print args

    def __call__(self, dts):
        print dts

init = Censor
