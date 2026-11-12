from audiofilters import Filter
from audiofilterhelper import synth_test, sine8k
from audiomixer import Mixer
from synthio import LFO


@synth_test
def stereo_filter():
    args = {
        "bits_per_sample": 16,
        "samples_signed": True,
        "channel_count": 2,
    }
    effect = Filter(**args)
    mixer = Mixer(**args)
    mixer.voice[0].panning = LFO()
    effect.play(mixer)
    yield effect, []

    mixer.play(sine8k, loop=True)
    yield 40

    mixer.stop_voice()
    yield 20
