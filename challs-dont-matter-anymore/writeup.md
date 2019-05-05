```python
import wave
import matplotlib.pyplot as plt
import struct

CODE_LENGTH = 32
assert CODE_LENGTH%2 == 0
N_HOSTS = 8
N_BITS = 880

wav = wave.open('CDMA_interception.wav', 'r')
raw_signal=[]
for i in range(N_BITS*CODE_LENGTH):
    frames = wav.readframes(100)
    samples = struct.unpack('h'*100, frames)
    raw_signal.append(sum(samples)/100)

buckets = [
    (2000, 6000),
    (8000, 13000),
    (15000, 21000),
    (22500, 28000),
    (30000, 36000),
    (38000, 43000),
    (45500, 50000),
    (53000, 57500),
    (60000, 63000)]

signal = []
for sig in raw_signal:
    for i in range(10):
        if buckets[i][0]<=sig+32768<=buckets[i][1]:
            signal.append(float(i)/N_HOSTS)
            break


codeset = set()

for veci in range(0,CODE_LENGTH*N_BITS,CODE_LENGTH):
    vec = [round(s*N_HOSTS) for s in signal[veci:veci+CODE_LENGTH]]
    codeset.add(tuple(vec))
print len(codeset)
    
codelist = list(codeset)

recovered_codes = set()
for i in range(len(codelist)):
    similar = []
    for j in range(len(codelist)):
        if i==j: continue
        if all(abs(b-a)<=1 for b,a in zip(codelist[i], codelist[j])):
            similar.append(i)
            recovered_codes.add(tuple([b-a for b,a in zip(codelist[i], codelist[j])]))

print len(recovered_codes)

for rc in recovered_codes:
    code = [bool(c+1.) for c in rc]

    recovered_sig = []
    for nbit, veci in enumerate(range(0,CODE_LENGTH*N_BITS,CODE_LENGTH)):
        vec = [round(s*N_HOSTS) for s in signal[veci:veci+CODE_LENGTH]]
        nvec = [v*2-5 for v in vec]
        res = sum(rci*nvi for rci, nvi in zip(rc, nvec))
        recovered_sig.append(res)
    assert len(set(recovered_sig)) == 2
    mx = max(recovered_sig)
    sig = ''
    ch = ''
    for rsg in recovered_sig:
        ch += '1' if rsg==mx else '0'
        if len(ch) == 8:
            sig += chr(int(ch,2))
            ch = ''
    print sig
    print '\n\n==================\n\n'
```