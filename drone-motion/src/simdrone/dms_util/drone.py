from time import sleep
from decimal import Decimal
from simdrone.common.log import app_log
from simdrone.common.vector import Vector3D
from simdrone.common.constants import PosZero
from simdrone.dms_util.model import Model
from simdrone.dms_util.track import Track

class Drone:
    '''[summary]
    '''
    def __init__(self, model, start=Track.REF):
        '''[summary]
        '''
        self._pos = start
        self._speed = PosZero
        self._accel = PosZero
        self._model = model
        self._total_time = 0

    def reach(self, target, drawer=None):
        '''[summary]
        '''
        i = 0
        app_log.debug("initial values:")
        app_log.debug(f"accel: {self._accel}")
        app_log.debug(f"speed: {self._speed}")
        app_log.debug(f"pos:   {self._pos}")
        while True:
            i += 1
            app_log.debug(f"iteration n°{i}:")
            if drawer:
                drawer.draw(self._pos)
            offset = target - self._pos
            if offset.length <= Decimal(1):
                break
            order = self._model.compute_next_order(offset, self._speed)
            direction = offset.normalized()
            self._accel = PosZero
            if order == Model.Order.SPEED_UP:
                self._accel = self._model.max_accel
            elif order == Model.Order.SLOW_DOWN:
                self._accel = -self._model.max_accel
            self._pos, self._speed = self._model.compute_next_pos(self._pos, direction, self._speed, self._accel)
            app_log.debug(f"dir: {direction}")
            app_log.debug(f"accel: {self._accel}")
            app_log.debug(f"speed: {self._speed}")
            app_log.debug(f"pos:   {self._pos}")
        app_log.debug(f"duration: {i * self._model.dt}s")
        app_log.debug(f"iterations: {i}")
        self._total_time += i * self._model.dt

    def land(self, drawer=None):
        '''[summary]
        '''
        if self._pos.z != PosZero:
            self.reach(Vector3D(self._pos.x, self._pos.y, PosZero), drawer)

    def follow(self, track, come_back=True, drawer=None):
        '''[summary]
        '''
        self._total_time = 0
        app_log.info("following track...")
        for waypoint in track.waypoints():
            app_log.info(f"going for {waypoint}")
            self.reach(waypoint, drawer)
            app_log.info(f"{waypoint} reached!")
        if come_back and track.end != track.start:
            self.reach(track.start, drawer)
        self.land(drawer)
        app_log.info(f"track performed in {self._total_time}s")
