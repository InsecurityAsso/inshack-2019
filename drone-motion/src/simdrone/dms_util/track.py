from ruamel.yaml import YAML
from simdrone.common.vector import Vector3D

class Track:
    '''[summary]
    '''
    REF = Vector3D(0, 0, 0)

    def __init__(self, track_fp):
        '''[summary]
        '''
        data = YAML(typ='safe').load(track_fp)
        self._track = []
        self._bounding_box = None
        for vect in data['track']:
            self._track.append(Vector3D(vect[0], vect[1], vect[2]))

    @property
    def start(self):
        '''[summary]
        '''
        return self._track[0]

    @property
    def end(self):
        '''[summary]
        '''
        return self._track[-1]

    @property
    def bounding_box(self):
        x_min, x_max = 0, 0
        y_min, y_max = 0, 0
        z_min, z_max = 0, 0
        if not self._bounding_box:
            for waypoint in self._track:
                x_min = min(x_min, waypoint.x)
                x_max = max(x_max, waypoint.x)
                y_min = min(y_min, waypoint.y)
                y_max = max(y_max, waypoint.y)
                z_min = min(z_min, waypoint.z)
                z_max = max(z_max, waypoint.z)
            self._bounding_box = (Vector3D(x_min, y_min, z_min),
                                  Vector3D(x_max, y_max, z_max))
        return self._bounding_box


    def waypoints(self):
        '''[summary]
        '''
        return iter(self._track)
