"""
toy utils.py

sqlite and enip use name (string) and pid (int) has key and the state stores
values as strings.

sqlite uses float keyword and cpppo use REAL keyword.

if ACTUATORX is 1 then is ON otherwise is OFF.
"""

from minicps.utils import build_debug_logger

toy_logger = build_debug_logger(
    name=__name__,
    bytes_per_file=10000,
    rotating_files=2,
    lformat='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    ldir='logs/',
    suffix='')


# others
PLC1_DATA = {
    'SENSOR1': '0',
    'SENSOR2': '0',
    'COUNTER': '0'
}

# protocol
PLC1_MAC = '00:00:54:AE:37:01'
PLC1_TAGS = (10, 10, 10, 10)
PLC1_ADDR = '10.0.0.3'
PLC1_SERVER = {
    'address': PLC1_ADDR,
    'tags': PLC1_TAGS
}
PLC1_PROTOCOL = {
    'name': 'modbus',
    'mode': 1,
    'server': PLC1_SERVER
}

SCADA_MAC = '64:00:6A:74:69:80'
SCADA_ADDR = '10.0.0.1'
SCADA_TAGS = (10, 10, 10, 10)  # TODO
SCADA_SERVER = {
    'address': SCADA_ADDR,
    'tags': SCADA_TAGS
}
SCADA_PROTOCOL = {
    'name': 'modbus',
    'mode': 1,
    'server': SCADA_SERVER
}

NETMASK = '/24'

# state
PATH = 'toy_db.sqlite'
NAME = 'toy_table'

STATE = {
    'name': NAME,
    'path': PATH
}

SCHEMA = """
CREATE TABLE toy_table (
    name              TEXT NOT NULL,
    datatype          TEXT NOT NULL,
    value             TEXT,
    pid               INTEGER NOT NULL,
    PRIMARY KEY (name, pid)
);
"""
SCHEMA_INIT = """
    INSERT INTO toy_table VALUES ('SENSOR1',   'int', '0', 1);
    INSERT INTO toy_table VALUES ('SENSOR2',   'int', '0', 1);
    INSERT INTO toy_table VALUES ('COUNTER',   'int', '0', 1);
"""
