"""
scada.py
"""
from pymodbus.client.sync import ModbusTcpClient as ModbusClient

from pymodbus.mei_message import *
from pymodbus.other_message import *
from minicps.devices import SCADAServer
from utils import SCADA_PROTOCOL, STATE, SCADA_ADDR
from utils import PLC1_ADDR, PLC1_TAGS

import time

PLC1_ADDR_1 = PLC1_ADDR + ':502'
SCADA_ADDR_1 = SCADA_ADDR + ':502'

class SCADAServer(SCADAServer):

    def pre_loop(self, sleep=0.5):
        """scada pre loop.

            - sleep
        """

        time.sleep(sleep)


    def main_loop(self):
        """scada main loop.

        For each RTU in the network
            - Read the pump status
        """
        client = ModbusClient('10.0.0.3', port=502)
        client.connect()
        while(True):
            #X = self.receive(('HR', 2, '2a'), PLC1_ADDR_1)



            ### TEST HOLDING REGISTERS slave 2###
            rr = client.read_holding_registers(0,3, unit=0x00)
            print "\nTest holding register slave0 :"
            print "\tFunction Code = %s" % rr.function_code
            if rr.registers:
                for i in range(3):

                    print "\tValue = %s" % rr.registers[i]


            # print("DEBUG: scada main loop")
            time.sleep(1)


if __name__ == "__main__":

    scada = SCADAServer(
        name='scada',
        state=STATE,
        protocol=SCADA_PROTOCOL)
