import subprocess
p = subprocess.check_output('openssl prime -generate -bits 2048 -hex')
q = subprocess.check_output('openssl prime -generate -bits 2048 -hex')
flag = int('INSA{REDACTED}'.encode('hex'), 16)

N = int(p,16) * int(q,16)
print N
print '0x'+p.replace('12','8D').replace('33','D4').replace('5E','FF').replace('09','95').replace('E4','38').replace('6B','89').replace('9E','E0').replace('59','3E')
print pow(flag,65537,N)