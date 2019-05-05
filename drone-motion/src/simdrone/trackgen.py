#!/usr/bin/env python3
from pathlib import Path
from decimal import getcontext
from argparse import ArgumentParser
from ruamel.yaml import YAML
from simdrone.common.log import app_log, enable_debug, disable_logging
from simdrone.trackgen_util.text import Text

def parse_args():
    '''[summary]
    '''
    p = ArgumentParser(description="Simulates drone's sensors output based on a given trajectory.")
    p.add_argument('--debug', '-d', action='store_true', help="Enable debugging mode.")
    p.add_argument('--silent', '-s', action='store_true', help="Enable silent mode.")
    p.add_argument('--precision', '-p', type=int, default=5, help="Set decimal precision.")
    p.add_argument('--ratio', '-r', type=int, default=1, help="Ratio to be applied on font chars.")
    p.add_argument('--sbc', '-e', type=int, default=2, help="Space between chars.")
    p.add_argument('text', help="Text to be converted to track.")
    p.add_argument('track', type=Path, help="Path to track file to be created.")
    return p.parse_args()

def app():
    '''[summary]
    '''
    args = parse_args()
    if args.silent:
        disable_logging()
    else:
        enable_debug(args.debug)
    app_log.debug(f"Arguments: {args}")

    getcontext().prec = args.precision

    text = Text(args.text.upper(), args.sbc, args.ratio)

    yaml = YAML(typ='safe')
    yaml.default_control_flow = False
    yaml.indent = 2
    yaml.dump({'track': [[float(p.x), float(p.y), float(250)] for p in text.points()]}, args.track)

if __name__ == '__main__':
    app()
