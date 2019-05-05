from math import pow, sqrt
from decimal import Decimal, getcontext

CTX = getcontext()

class Vector3D:
    '''[summary]
    '''
    def __init__(self, x, y, z):
        '''[summary]
        '''
        self.x = Decimal(x)
        self.y = Decimal(y)
        self.z = Decimal(z)

    def __str__(self):
        '''[summary]
        '''
        return f"(x={float(self.x):.5f},y={float(self.y):.5f},z={float(self.z):.5f})"

    def __add__(self, other):
        '''[summary]
        '''
        return Vector3D(CTX.add(self.x, other.x),
                        CTX.add(self.y, other.y),
                        CTX.add(self.z, other.z))

    def __sub__(self, other):
        '''[summary]
        '''
        return Vector3D(CTX.subtract(self.x, other.x),
                        CTX.subtract(self.y, other.y),
                        CTX.subtract(self.z, other.z))

    def __mul__(self, factor):
        '''[summary]
        '''
        factor = Decimal(factor)
        return Vector3D(CTX.multiply(self.x, factor),
                        CTX.multiply(self.y, factor),
                        CTX.multiply(self.z, factor))

    def __ne__(self, other):
        '''[summary]
        '''
        return self.x != other.x or self.y != other.y or self.z != other.z

    def __eq__(self, other):
        '''[summary]
        '''
        return not self.__ne__(other)

    @property
    def length(self):
        '''[summary]
        '''
        return CTX.sqrt(CTX.power(self.x, 2) + CTX.power(self.y, 2) + CTX.power(self.z, 2))

    def normalized(self):
        '''[summary]
        '''
        factor = Decimal(1) / self.length
        return Vector3D(CTX.multiply(self.x, factor),
                        CTX.multiply(self.y, factor),
                        CTX.multiply(self.z, factor))

    def normalize(self):
        '''[summary]
        '''
        self = self.normalized
        return self

class Vector2D:
    '''[summary]
    '''
    def __init__(self, x, y):
        '''[summary]
        '''
        self.x = Decimal(x)
        self.y = Decimal(y)

    def __str__(self):
        '''[summary]
        '''
        return f"(x={float(self.x):.5f},y={float(self.y):.5f})"

    def __add__(self, other):
        '''[summary]
        '''
        return Vector2D(CTX.add(self.x, other.x),
                        CTX.add(self.y, other.y))

    def __sub__(self, other):
        '''[summary]
        '''
        return Vector2D(CTX.subtract(self.x, other.x),
                        CTX.subtract(self.y, other.y))

    def __mul__(self, factor):
        '''[summary]
        '''
        factor = Decimal(factor)
        return Vector2D(CTX.multiply(self.x, factor),
                        CTX.multiply(self.y, factor))

    def __ne__(self, other):
        '''[summary]
        '''
        return self.x != other.x or self.y != other.y

    def __eq__(self, other):
        '''[summary]
        '''
        return not self.__ne__(other)

    @property
    def length(self):
        '''[summary]
        '''
        return CTX.sqrt(CTX.power(self.x, 2) + CTX.power(self.y, 2))

    def normalized(self):
        '''[summary]
        '''
        factor = Decimal(1) / self.length
        return Vector2D(CTX.multiply(self.x, factor),
                        CTX.multiply(self.y, factor))

    def normalize(self):
        '''[summary]
        '''
        self = self.normalized
        return self
