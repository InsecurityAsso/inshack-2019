import re
import os
import logging
import pathlib
import argparse
import binascii

FORMAT = 'resgen[%(levelname)7s]: %(message)s'
logging.basicConfig(format=FORMAT)
app_log = logging.getLogger('resgen')
app_log.setLevel(logging.DEBUG)
RES_RE = re.compile(r'(?P<idx>\d+):"(?P<data>[^"]*)"')

def parse_strings_res(filepath):
    resources = []
    with filepath.open('r') as strings:
        for string in strings:
            match = RES_RE.match(string)
            if not match:
                raise RuntimeError("Invalid input resource string.")
            resources.append((match.group('idx'), match.group('data')))
    return sorted(resources, key=lambda t: t[0])

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--key', '-k',
                        default=os.urandom(32).hex(),
                        help="Hexadecimal key to be used for generation")
    parser.add_argument('--chall-conf', '-c', type=pathlib.Path,
                        help="Path to challenge configuration file")
    parser.add_argument('--header-file', type=pathlib.Path,
                        default=pathlib.Path('strings.res'),
                        help="Path to header file")
    parser.add_argument('--footer-file', type=pathlib.Path,
                        default=pathlib.Path('strings.res'),
                        help="Path to footer file")
    parser.add_argument('--strings-file', '-r', type=pathlib.Path,
                        default=pathlib.Path('strings.res'),
                        help="Path to strings.res file")
    parser.add_argument('--output-file', '-o', type=pathlib.Path,
                        default=pathlib.Path('strings.res'),
                        help="Path to output file")
    return parser.parse_args()

def xor(key, data):
    if not data:
        return []
    return [a ^ b for a, b in zip(data, key * ((len(data) // len(key)) + 1))]

def encode_res(key, resources):
    encoded_res = []
    for _, data in resources:
        if '\\n' in data:
            data = data.replace('\\n', '\n')
        encoded = xor(key, list(data.encode()))
        encoded_res.append(encoded)
        app_log.debug(f"encrypting: {bytes(xor(key, encoded))} -> {bytes(encoded)}")
    return encoded_res

def compute_res(key, flag):
    failure = 'H4x0r says: nope.\n'
    success = f'H4x0r says: good job human, {flag}\n'
    return [
        xor(key, failure.encode()),
        xor(key, success.encode())
    ]

def generate_c_array(data):
    if data:
        array = ', '.join([hex(b) for b in data])
    else:
        array = '0'
    return '{' + array + '}'

def generate_code(key, encoded_res, computed_res):
    t = ' ' * 4
    code = []
    code.append("static const buf_t gresk={")
    code.append(f"{t}.size={len(key)},")
    code.append(f"{t}.data={generate_c_array(key)}")
    code.append("};")
    code.append(f"static const int gresd_sz={len(encoded_res)};")
    code.append(f"static const buf_t gresd_data[{len(encoded_res)}]={{")
    k = 0
    base_idx = 0
    for resource in encoded_res:
        code.append(f"{t}{{")
        code.append(f"{t}{t}.size={len(resource)},")
        code.append(f"{t}{t}.data={generate_c_array(resource)}")
        code.append(f"{t}}},")
        k += 1
        if len(resource) == 0:
            base_idx = k
    code.append("};")
    code.append(f"static int gbase_idx={base_idx};")
    code.append(f"static const int gcfailure=0;")
    code.append(f"static const int gcsuccess=1;")
    code.append(f"static const buf_t gcresd[{len(computed_res)}]={{")
    for resource in computed_res:
        code.append(f"{t}{{")
        code.append(f"{t}{t}.size={len(resource)},")
        code.append(f"{t}{t}.data={generate_c_array(resource)}")
        code.append(f"{t}}},")
    code.append("};")
    return '\n'.join(code)

def load_flag(chall_conf):
    if not chall_conf:
        return 'INSA{}'
    with chall_conf.open() as cfp:
        for line in cfp:
            if 'flag:' in line:
                return line.split(':')[-1].strip()

def app():
    app_log.info(r'''
 ____            ____
|  _ \ ___  ___ / ___| ___ _ __
| |_) / _ \/ __| |  _ / _ \ '_ \
|  _ <  __/\__ \ |_| |  __/ | | |
|_| \_\___||___/\____|\___|_| |_|

''')
    args = parse_args()
    key = list(binascii.unhexlify(args.key))
    flag = load_flag(args.chall_conf)
    app_log.debug(f"injecting flag: {flag}")
    resources = parse_strings_res(args.strings_file)
    computed_res = compute_res(key, flag)
    encoded_res = encode_res(key, resources)
    with args.output_file.open('w') as ofp:
        ofp.write(args.header_file.read_text())
        ofp.write(generate_code(key, encoded_res, computed_res))
        ofp.write(args.footer_file.read_text())

if __name__ == '__main__':
    app()
