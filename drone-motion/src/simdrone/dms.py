#!/usr/bin/env python3
from pathlib import Path
from decimal import getcontext
from argparse import ArgumentParser
from simdrone.common.log import (
    disable_logging,
    enable_debug,
    log_to_file,
    app_log
)
from simdrone.dms_util.drone import Drone
from simdrone.dms_util.track import Track
from simdrone.dms_util.model import Model
from simdrone.dms_util.trajectory_drawer import TrajectoryDrawer

def parse_args():
    '''[summary]
    '''
    p = ArgumentParser(description="Simulates drone's sensors output based on a given trajectory.")
    p.add_argument('--log', '-l', type=Path, help="Path to the log file.")
    p.add_argument('--debug', '-d', action='store_true', help="Enable debugging mode.")
    p.add_argument('--silent', '-s', action='store_true', help="Enable silent mode.")
    p.add_argument('--precision', '-p', type=int, default=5, help="Set decimal precision.")
    p.add_argument('--draw', '-o', type=Path, help="Path the image to be created.")
    p.add_argument('--margin', '-m', type=int, default=64, help="Margins to be added to image.")
    p.add_argument('model', type=Path, help="Path to drone model file.")
    p.add_argument('track', type=Path, help="Path to drone track file.")
    return p.parse_args()

def app():
    '''[summary]
    '''
    args = parse_args()
    if args.silent:
        disable_logging()
    else:
        enable_debug(args.debug)

    if args.log:
        log_to_file(args.log)

    app_log.debug(f"Arguments: {args}")

    getcontext().prec = args.precision

    model = Model(args.model)
    track = Track(args.track)
    drone = Drone(model)

    drawer = None
    if args.draw:
        drawer = TrajectoryDrawer(track, args.margin)

    drone.follow(track, come_back=False, drawer=drawer)

    if args.draw:
        drawer.save(args.draw)

if __name__ == '__main__':
    app()
