import math
import random
import struct
import wave

CARRIER_FREQ = 100000
CARRIER_SAMPLING = 960000

SIGNAL_FREQ = 440
SIGNAL_SAMPLING = 48000

assert CARRIER_SAMPLING % SIGNAL_SAMPLING == 0

FM_FLAG = '010010010100111001010011010000010111101101101001010100110101111101101001011011100101111101110100011010000011001101011111010000010110100101010010010111110011010001011111011101010101111100110100011011100110010001011111010011010110010101111101'
wav = wave.open('amsignal_mono.wav', 'r')

nsamples = wav.getnframes()
frames = wav.readframes(nsamples)
am_samples = struct.unpack('h'*nsamples, frames)

prev_sample = 0.0
signal = []
flagptr = 55
for sigi, am_sample in enumerate(am_samples):
    am01 = (am_sample+32768)/65536.
    assert 0.0<=am01<=1.0
    for i in range(CARRIER_SAMPLING / SIGNAL_SAMPLING):
        t = float(sigi) / SIGNAL_SAMPLING + float(i) / CARRIER_SAMPLING
        prevratio = SIGNAL_SAMPLING * (float(i) / CARRIER_SAMPLING)
        sample = prev_sample * prevratio + am01 * (1-prevratio)
        sample *= math.sin(t * 0.5 * CARRIER_FREQ * (1 + int(FM_FLAG[flagptr])) * 2 * math.pi + 0.44)
        signal.append(sample)
    prev_sample = am01
    flagptr += 1
    flagptr %= len(FM_FLAG)

'''   
signal = []
for sigi in range(DURATION * SIGNAL_SAMPLING):
    t = float(sigi) / SIGNAL_SAMPLING
    sample = math.sin(t * SIGNAL_FREQ * 2 * math.pi + 0.44)
    signal.append(sample)
'''
print signal[:100]

output_wav = wave.open('AFM.wav', 'w')
output_wav.setparams((1, 2, CARRIER_SAMPLING, 0, 'NONE', 'not compressed'))

packed_values=[]
for sample in signal[45:]:
    value = int(sample*32760)
    packed_values.append(struct.pack('h', value))
output_wav.writeframes(''.join(packed_values))
output_wav.close()

