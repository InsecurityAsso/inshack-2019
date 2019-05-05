import wave
import struct
import random

CODE_LENGTH = 32
assert CODE_LENGTH%2 == 0
N_HOSTS = 8
N_BITS = 880

def code2num(code):
    res = 0
    for e in code:
        res *=2
        res += int(e)
    return res

def isorth(v1, v2):
    assert len(v1) == len(v2)
    score = 0
    for a,b in zip(v1,v2):
        score += 1 if a ^ b else -1
    return score == 0

def bf_codes(signal):
    scores = []
    for candidatecode in range(2**CODE_LENGTH):
        code = []
        score = 0.0
        for i in range(CODE_LENGTH):
            code.append(bool((candidatecode>>i)&1))
        for veci in range(0,CODE_LENGTH*N_BITS,CODE_LENGTH):
            localscore = 0.0
            for i in range(CODE_LENGTH):
                if code[i]:
                    localscore += signal[veci+i]
                else:
                    localscore += 1.0 - signal[veci+i]
            score += localscore**2

        scores.append((score, code))

    scores.sort(reverse=True)
    for score, code in scores[:10]:
        print score, code2num(code), code

codes = []

while len(codes) != N_HOSTS:
    code = []
    for j in range(CODE_LENGTH):
        code.append(bool(random.getrandbits(1)))
    if all(isorth(code, c) for c in codes):
        print 'Found orthogonal code'
        codes.append(code)

data1 = "Oh hi, I see you've found my secret device. Want a flag ? Here you go INSA{Elektr0nenklange_Aus_d3m_R4di0l4nD}"
data1 = ''.join(format(ord(i),'b').zfill(8) for i in data1)
print len(data1)
assert len(data1) == N_BITS

signal = []
for biti in range(N_BITS):
    vec = [0. for _ in range(CODE_LENGTH)]
    for host in range(N_HOSTS):
        if host == 0:
            bit = bool(int(data1[biti]))
        else:
            bit = bool(random.getrandbits(1))
        if host == 0 and biti < 30:
            print int(bit),
        for i, codebit in enumerate(codes[host]):
            vec[i] += (1.0/N_HOSTS) * int(codebit^bit)

    signal.extend(vec)
print ''

output_wav = wave.open('chal2.wav', 'w')
output_wav.setparams((1, 2, 8000000, 0, 'NONE', 'not compressed'))

packed_values=[]
for sample in signal:
    for i in range(100):
        value = int(sample*60000-30000) + random.randint(-10000, +10000)
        value = max(value, -32768)
        value = min(value, 32767)
        packed_values.append(struct.pack('h', value))
output_wav.writeframes(''.join(packed_values))
output_wav.close()

for code in codes:
    print code2num(code)
print ''

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
    print code2num(code)

    recovered_sig = []
    for nbit, veci in enumerate(range(0,CODE_LENGTH*N_BITS,CODE_LENGTH)):
        vec = [round(s*N_HOSTS) for s in signal[veci:veci+CODE_LENGTH]]
        nvec = [v*2-5 for v in vec]
        res = sum(rci*nvi for rci, nvi in zip(rc, nvec))
        recovered_sig.append(res)
        if nbit > 30:
            print ''
            break
    assert len(set(recovered_sig)) == 2
    mx = max(recovered_sig)
    for rsg in recovered_sig:
        print 1 if rsg==mx else 0,
    print ''
