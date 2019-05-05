import os
import time
import enum
import json
import socket
import argparse
import traceback
import logging as app_log
from scapy.all import *

app_log.basicConfig(level=app_log.DEBUG,
                    format='(%(asctime)s)[%(levelname)7s]: %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S')

PROTO = {
    'TCP': TCP,
    'UDP': UDP
}

class Configuration:
    '''[summary]
    '''
    def __init__(self, config_path):
        '''Create a configuration
        '''
        self._conf = {}
        with open(config_path, 'r') as fp:
            self._conf = json.load(fp)
        app_log.debug(json.dumps(self._conf, indent=2))

    @property
    def door(self):
        '''Return door port
        '''
        return self._conf['door']

    @property
    def seq_interval(self):
        '''Return sequence interval in seconds
        '''
        return self._conf['seq_interval']

    @property
    def door_interval(self):
        '''Return door interval in seconds
        '''
        return self._conf['door_interval']

    @property
    def sequence(self):
        '''Return sequence
        '''
        return self._conf['sequence']

    @property
    def open_sesame(self):
        '''Return open sesame
        '''
        return self._conf['open_sesame'].encode()

    @property
    def sequence_length(self):
        '''Return sequence length
        '''
        return len(self.sequence)

class Sesame:
    '''[summary]
    '''
    class State(enum.Enum):
        '''[summary]
        '''
        IDLE = 'IDLE'
        RECORDING = 'RECORDING'

    def __init__(self, conf, flag):
        '''Create a Sesame based on given configuration and flag
        '''
        self._conf = conf
        self._flag = flag
        self._state = self._idle()
        self._start_time = 0
        self._whitelisted_ip = ''

    @property
    def state(self):
        '''Return current Sesame state
        '''
        return self._state

    @property
    def current_stage_conditions(self):
        '''Return current stage conditions to be met
        '''
        return self._conf.sequence[self._stage]

    def _idle(self):
        '''Reset counters for next sequence
        '''
        self._stage = 0
        self._start_time = 0
        return Sesame.State.IDLE

    def _check_sequence_timeout(self):
        '''Check if
        '''
        delta = time.time() - self._start_time
        if delta > self._conf.seq_interval:
            self._state = self._idle()
            app_log.info(f"sequence interval timed out, current state is {self._state}.")

    def _handle_first_knock(self, dport, src_ip):
        '''Handle first knock of the sequence
        '''
        app_log.info(f"{src_ip} is beginning the sequence with {dport}.")
        self._stage += 1
        self._start_time = time.time()
        self._whitelisted_ip = src_ip
        return Sesame.State.RECORDING

    def _handle_knock(self, dport, src_ip):
        '''Handle all but first knock in the sequence
        '''
        app_log.info(f"{src_ip} successfully knocked at {dport}")
        self._stage += 1
        if self._stage >= self._conf.sequence_length:
            self._open_door()
            return self._idle()
        return None

    def _open_door(self):
        '''[summary]
        '''
        app_log.info(f"opening door ({self._conf.door}) for {self._conf.door_interval}s...")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind(('0.0.0.0', self._conf.door))
            sock.settimeout(self._conf.door_interval)
            sock.listen()
            try:
                # accept connection from currently whitelisted ip only
                src_ip = None
                while True:
                    conn, addr = sock.accept()
                    src_ip = addr[0]
                    if src_ip == self._whitelisted_ip:
                        app_log.info(f"accepted connection from {src_ip}.")
                        break
                    app_log.info(f"rejected connection from {src_ip}.")
                # process connection
                data = b''
                while True:
                    data = conn.recv(1024)
                    if not data:
                        break
                    if self._conf.open_sesame in data:
                        app_log.info(f"{src_ip} solved the challenge.")
                        conn.sendall(self._flag.encode())
                    else:
                        conn.sendall(b'ASK POLITELY THIEF!')
                    break
                conn.close()
            except socket.timeout:
                app_log.info("socket timeout catched.")
        except:
            traceback.print_exc()
        finally:
            sock.close()
        app_log.info("door closed.")

    def _update_state(self, dport, src_ip):
        '''Update Sesame state based on checks and current state
        '''
        # check if previous sequence has expired
        self._check_sequence_timeout()
        # compute next state
        next_state = None
        if self.state == Sesame.State.IDLE:
            next_state = self._handle_first_knock(dport, src_ip)
        elif self.state == Sesame.State.RECORDING:
            next_state = self._handle_knock(dport, src_ip)
        else:
            raise RuntimeError(f"unhandled state: {self.state}")
        if next_state:
            app_log.info(f"transition: {self.state} -> {next_state}")
            self._state = next_state

    def greet_visitor(self, pkt):
        '''Process incoming packets
        '''
        dport = pkt.payload.payload.dport
        proto = pkt.haslayer(PROTO[self.current_stage_conditions['proto']])
        src_ip = pkt.payload.src
        if proto and dport == self.current_stage_conditions['port']:
            # anyone having good proto and port triggers a state update
            # source ip and sequence interval are checked later...
            self._update_state(dport, src_ip)

def parse_args():
    '''Parse script arguments
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('iface', help="Interface to sniff on. Defaults to all interfaces.")
    parser.add_argument('config', type=Configuration, help="Path to Sesame configuration file.")
    return parser.parse_args()

def app():
    '''Application entry point
    '''
    args = parse_args()
    app_log.debug(args)
    groom = Sesame(args.config, os.getenv('SESAME_FLAG', 'DEFAULT_SESAME_FLAG'))
    try:
        sniff(count=0,
              iface=args.iface,
              filter='udp or tcp',
              stop_filter=groom.greet_visitor)
    except:
        app_log.exception("An exception occured (details below) :/")

if __name__ == '__main__':
    print(r'''
                                           /|
                                          / |
                                         /__|______
                                        |  __  __  |
 ____                                   | |  ||  | |                    | ,
/ ___|  ___  ___  __ _ _ __ ___   ___   | |__||__| |   _  _ |> |>[ [ |\ |/
\___ \ / _ \/ __|/ _` | '_ ` _ \ / _ \  |  __  __()|  (  (_ |\ |\[ [ |-\|\
 ___) |  __/\__ \ (_| | | | | | |  __/  | |  ||  | |  _)    |           | \
|____/ \___||___/\__,_|_| |_| |_|\___|  | |  ||  | |
                                        | |__||__| |
                                        |__________| ejm
''')
    app()
