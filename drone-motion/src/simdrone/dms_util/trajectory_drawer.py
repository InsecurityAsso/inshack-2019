from PIL import Image, ImageDraw
from simdrone.common.vector import Vector2D

class TrajectoryDrawer:
    '''[summary]
    '''
    MODE = 'RGB'
    BGCOLOR = 'white'
    PENCOLOR = 'black'

    def __init__(self, track, margin):
        '''[summary]
        '''
        self._margin = margin
        self._bbox = track.bounding_box
        self._x_im = Image.new(TrajectoryDrawer.MODE,
                               (self._bbox[1].y - self._bbox[0].y + 2*margin,
                                self._bbox[1].z - self._bbox[0].z + 2*margin),
                               color=TrajectoryDrawer.BGCOLOR)
        self._y_im = Image.new(TrajectoryDrawer.MODE,
                               (self._bbox[1].x - self._bbox[0].x + 2*margin,
                                self._bbox[1].z - self._bbox[0].z + 2*margin),
                               color=TrajectoryDrawer.BGCOLOR)
        self._z_im = Image.new(TrajectoryDrawer.MODE,
                               (self._bbox[1].x - self._bbox[0].x + 2*margin,
                                self._bbox[1].y - self._bbox[0].y + 2*margin),
                               color=TrajectoryDrawer.BGCOLOR)
        self._x_drawer = ImageDraw.Draw(self._x_im, TrajectoryDrawer.MODE)
        self._y_drawer = ImageDraw.Draw(self._y_im, TrajectoryDrawer.MODE)
        self._z_drawer = ImageDraw.Draw(self._z_im, TrajectoryDrawer.MODE)

    def draw(self, vect):
        '''[summary]
        '''
        origin = Vector2D(self._margin, -self._margin)
        x_point = origin + Vector2D(abs(self._bbox[0].y),
                                    -abs(self._bbox[0].z)) + Vector2D(vect.y,
                                                                      -vect.z+self._x_im.height)
        y_point = origin + Vector2D(abs(self._bbox[0].x),
                                    -abs(self._bbox[0].z)) + Vector2D(vect.x,
                                                                      -vect.z+self._y_im.height)
        z_point = origin + Vector2D(abs(self._bbox[0].x),
                                    -abs(self._bbox[0].y)) + Vector2D(vect.x,
                                                                      -vect.y+self._z_im.height)
        self._x_drawer.point((x_point.x, x_point.y), fill=TrajectoryDrawer.PENCOLOR)
        self._y_drawer.point((y_point.x, y_point.y), fill=TrajectoryDrawer.PENCOLOR)
        self._z_drawer.point((z_point.x, z_point.y), fill=TrajectoryDrawer.PENCOLOR)

    def save(self, base_fp):
        x_base_fp = base_fp.parent.joinpath(f'{base_fp.stem}.x{base_fp.suffix}')
        y_base_fp = base_fp.parent.joinpath(f'{base_fp.stem}.y{base_fp.suffix}')
        z_base_fp = base_fp.parent.joinpath(f'{base_fp.stem}.z{base_fp.suffix}')
        self._x_im.save(x_base_fp)
        self._y_im.save(y_base_fp)
        self._z_im.save(z_base_fp)
