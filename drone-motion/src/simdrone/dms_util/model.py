from enum import Enum
from decimal import Decimal, getcontext
from ruamel.yaml import YAML
from simdrone.common.log import app_log
from simdrone.common.constants import PosZero

CTX = getcontext()

class Model:
    '''[summary]
    '''
    class Order(Enum):
        '''[summary]
        '''
        SPEED_UP = 0
        SLOW_DOWN = 1
        KEEP_GOING = 2

    def __init__(self, model_fp):
        '''[summary]
        '''
        data = YAML(typ='safe').load(model_fp)
        # members initialization
        self.max_accel = Decimal(data['max-accel'])
        self.max_speed = Decimal(data['max-speed'])
        # some checks
        assert self.max_accel > PosZero
        assert self.max_speed > PosZero
        # minimum dt to have a 1 m/s velocity resolution
        self.dt = CTX.divide(1, self.max_accel)
        app_log.debug(f"using dt={self.dt}s")
        # deceleration time to full stop
        self.dec_dt_cnt = CTX.divide(self.max_speed, self.max_accel * self.dt)

    def compute_next_speed(self, speed, accel):
        '''[summary]
        '''
        speed = accel * self.dt + speed
        if speed > self.max_speed:
            return self.max_speed
        return speed

    def compute_next_pos(self, pos, direction, speed, accel):
        '''[summary]
        '''
        speed = self.compute_next_speed(speed, accel)
        pos += direction * (speed * self.dt)
        return pos, speed

    def compute_next_order(self, offset, speed):
        '''[summary]
        '''
        if offset.length > self.dec_dt_cnt * speed:
            return Model.Order.SPEED_UP
        else:
            return Model.Order.SLOW_DOWN
