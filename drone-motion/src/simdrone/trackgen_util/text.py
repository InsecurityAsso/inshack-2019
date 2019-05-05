from simdrone.common.vector import Vector2D
from simdrone.trackgen_util.font import CHRMAP

VALID_CHARS = list(CHRMAP.keys())

class Text:
    '''[summary]
    '''
    def __init__(self, string, sbc=2, ratio=1):
        for l in string:
            if not l in VALID_CHARS:
                raise ValueError(f"{l} if not part of valid charset: {VALID_CHARS}")
        self._sbc = sbc
        self._ratio = ratio
        self._string = string

    def points(self):
        '''[summary]
        '''
        offset = 0
        for l in self._string:
            char = CHRMAP[l]
            for point in char.points:
                yield (point * self._ratio) + Vector2D(offset, 0)
            br = char.bounding_rect
            width = br[1].x - br[0].x
            offset += (width + self._sbc) * self._ratio
