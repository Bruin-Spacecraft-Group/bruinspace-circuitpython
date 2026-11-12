from synthnotehelper import *


@synth_test(channel_count=2)
def gen(synth):
    l = LFO(sine, offset=0.0, scale=1.0, rate=2)
    yield [l]
    n = Note(8, panning=l)
    synth.press(n)
    yield 2
