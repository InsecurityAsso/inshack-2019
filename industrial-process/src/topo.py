"""
toy topology
"""

from mininet.topo import Topo

from utils import PLC1_MAC, SCADA_MAC
from utils import PLC1_ADDR, SCADA_ADDR, NETMASK


class ToyTopo(Topo):

    def build(self):

        switch = self.addSwitch('s1')

        plc1 = self.addHost(
            'plc1',
            ip=PLC1_ADDR + NETMASK,
            mac=PLC1_MAC)
        self.addLink(plc1, switch)

        SCADA = self.addHost(
            'SCADA',
            ip=SCADA_ADDR + NETMASK,
            mac=SCADA_MAC)
        self.addLink(SCADA, switch)
