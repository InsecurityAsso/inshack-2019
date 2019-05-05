import argparse
from scapy.sendrecv import sniff

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('iface', help="Interface to sniff on. Defaults to all interfaces.")
    parser.add_argument('--count', '-c', type=int, default=0, help="Count of packets to sniff. 0 means infinite.")
    parser.add_argument('--filter', '-f', default='tcp or udp', help="Filter to apply when sniffing...")
    return parser.parse_args()

if __name__ == '__main__':
    args = parse_args()
    sniff(count=args.count, iface=args.iface, filter=args.filter, stop_filter=lambda p: p.show())
