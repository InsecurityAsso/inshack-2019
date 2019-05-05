import re
import os
import logging
import pathlib
import argparse
import binascii

FORMAT = 'obfuscate[%(levelname)7s]: %(message)s'
logging.basicConfig(format=FORMAT)
app_log = logging.getLogger('obfuscate')
app_log.setLevel(logging.DEBUG)

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-file', '-i', type=pathlib.Path,
                        default=pathlib.Path('tob.c'),
                        help="Input file")
    parser.add_argument('--output-file', '-o', type=pathlib.Path,
                        default=pathlib.Path('tob.obf.c'),
                        help="Output file")
    parser.add_argument('--identifiers-file', type=pathlib.Path,
                        default=pathlib.Path('identifiers.res'),
                        help="identifiers file")
    return parser.parse_args()

def build_obfuscation_dict(identifiers_file):
    obfdict = {}
    randvset = set()
    with identifiers_file.open() as ifp:
        for line in ifp:
            while True:
                randv = f'_{os.urandom(4).hex()}'
                if randv not in randvset:
                    randvset.add(randv)
                    obfdict[line.strip()] = randv
                    break
    return obfdict

def obfuscated(line, obfdict):
    for k, v in obfdict.items():
        if k in line:
            app_log.debug(f"replacing: {k} -> {v}")
            line = line.replace(k, v)
    return line

def app():
    app_log.info(r'''
  ___  _      __                     _
 / _ \| |__  / _|_   _ ___  ___ __ _| |_ ___
| | | | '_ \| |_| | | / __|/ __/ _` | __/ _ \
| |_| | |_) |  _| |_| \__ \ (_| (_| | ||  __/
 \___/|_.__/|_|  \__,_|___/\___\__,_|\__\___|

''')
    args = parse_args()
    obfdict = build_obfuscation_dict(args.identifiers_file)
    with args.input_file.open() as ifp:
        with args.output_file.open('w') as ofp:
            for line in ifp:
                ofp.write(obfuscated(line, obfdict))

if __name__ == '__main__':
    app()
