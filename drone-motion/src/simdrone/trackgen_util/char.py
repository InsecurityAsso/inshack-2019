from simdrone.common.vector import Vector2D
from simdrone.common.constants import PosZero

class Char:
    '''[summary]
    '''
    def __init__(self, points):
        '''[summary]
        '''
        self._points = [Vector2D(x, y) for x, y in points]
        self._bounding_rect = None

    @property
    def first(self):
        '''[summary]
        '''
        return self._points[0]

    @property
    def last(self):
        '''[summary]
        '''
        return self._points[-1]

    @property
    def points(self):
        '''[summary]
        '''
        return self._points

    @property
    def bounding_rect(self):
        '''[summary]
        '''
        if not self._bounding_rect:
            x_min, x_max = PosZero, PosZero
            y_min, y_max = PosZero, PosZero
            for point in self.points:
                x_min = min(x_min, point.x)
                x_max = max(x_max, point.x)
                y_min = min(y_min, point.y)
                y_max = max(y_max, point.y)
            self._bounding_rect = [Vector2D(x_min, y_min), Vector2D(x_max, y_max)]
        return self._bounding_rect
