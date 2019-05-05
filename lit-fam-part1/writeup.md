```python
import struct
import wave

wav = wave.open('FAM.wav', 'r')
am_sig = []

nframes = wav.getnframes()
frames = wav.readframes(nframes)
samples = struct.unpack('h'*nframes, frames)
lastinvert = 0
sgn = 1
fm_sig=''
for i in range(15,nframes,20):
    sv = max(map(abs,samples[i:i+20]))
    am_sig.append(sv*2-32768)
    switchcnt = 0
    for j in range(19):
        if samples[i+j]*samples[i+j+1]<0:
            switchcnt += 1
    fm_sig += '1' if switchcnt > 2 else '0'


# INSA{iS_in_th3_AiR_4_u_4nd_Me}
for cut in range(8):
    s = fm_sig[cut:800+cut]
    print ''.join(chr(int(s[i*8:i*8+8],2)) for i in range(len(s)//8))


# INSA{RADIOACTIVITY}
output_wav = wave.open('extracted_am.wav', 'w')
output_wav.setparams((1, 2, 44100, 0, 'NONE', 'not compressed'))
packed_values=[]
for sample in am_sig:
    packed_values.append(struct.pack('h', sample))
output_wav.writeframes(''.join(packed_values))
output_wav.close()
```