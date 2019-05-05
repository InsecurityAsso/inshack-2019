from simdrone.common.vector import Vector2D
from simdrone.trackgen_util.char import Char

ORIGIN = (00.00, 00.00)

_A = [
    ORIGIN,
    (05.00, 12.00),
    (07.50, 06.00),
    (02.50, 06.00),
    (07.50, 06.00),
    (10.00, 00.00)
]
_B = [
    ORIGIN,
    (00.00, 12.00),
    (04.00, 11.75),
    (06.00, 10.00),
    (06.00, 09.00),
    (04.00, 07.00),
    (00.00, 06.00),
    (05.00, 06.00),
    (06.50, 04.00),
    (06.50, 02.00),
    (05.50, 00.50),
    ORIGIN
]
_C = [
    (02.00, 01.50),
    (01.00, 03.00),
    (00.00, 05.00),
    (00.00, 07.00),
    (01.00, 09.00),
    (02.00, 10.50),
    (04.00, 11.50),
    (06.50, 12.00),
    (08.00, 11.50),
    (06.50, 12.00),
    (04.00, 11.50),
    (02.00, 10.50),
    (01.00, 09.00),
    (00.00, 07.00),
    (00.00, 05.00),
    (01.00, 03.00),
    (02.00, 01.50),
    (04.00, 00.50),
    (06.50, 00.00),
    (08.00, 00.50)
]
_D = [
    ORIGIN,
    (00.00, 12.00),
    (03.00, 11.50),
    (05.00, 10.50),
    (06.50, 09.00),
    (07.00, 06.00),
    (06.50, 03.00),
    (05.00, 01.50),
    (03.00, 00.50),
    ORIGIN
]
_E = [
    ORIGIN,
    (00.00, 06.00),
    (07.00, 06.00),
    (00.00, 06.00),
    (00.00, 12.00),
    (07.00, 12.00),
    (00.00, 12.00),
    ORIGIN,
    (07.00, 00.00)
]
_F = [
    ORIGIN,
    (00.00, 06.00),
    (04.00, 06.00),
    (00.00, 06.00),
    (00.00, 12.00),
    (08.00, 12.00),
    (00.00, 12.00),
    ORIGIN
]
_I = [
    ORIGIN,
    (03.00, 00.00),
    (03.00, 12.00),
    (00.00, 12.00),
    (06.00, 12.00),
    (03.00, 12.00),
    (03.00, 00.00),
    (06.00, 00.00),
]
_N = [
    ORIGIN,
    (00.00, 12.00),
    (07.00, 00.00),
    (07.00, 12.00),
    (07.00, 00.00)
]
_S = [
    (00.00, 01.50),
    (02.00, 00.50),
    (04.00, 00.00),
    (06.00, 01.00),
    (08.00, 04.00),
    (07.00, 06.00),
    (05.00, 07.00),
    (02.00, 07.50),
    (00.00, 09.00),
    (01.00, 11.00),
    (05.00, 12.00),
    (08.00, 11.00),
    (05.00, 12.00),
    (01.00, 11.00),
    (00.00, 09.00),
    (02.00, 07.50),
    (05.00, 07.00),
    (07.00, 06.00),
    (08.00, 04.00),
    (06.00, 01.00)
]
_0 = [
    (02.00, 01.00),
    (04.00, 00.00),
    (06.00, 01.00),
    (04.00, 00.00),
    (02.00, 01.00),
    (01.00, 02.00),
    (00.50, 04.00),
    (00.00, 06.00),
    (00.50, 08.00),
    (01.00, 10.00),
    (02.00, 11.00),
    (04.00, 12.00),
    (06.00, 11.00),
    (07.00, 10.00),
    (07.50, 08.00),
    (08.00, 06.00),
    (07.50, 04.00),
    (07.00, 02.00),
    (06.00, 01.00)
]
_1 = [
    ORIGIN,
    (03.00, 00.00),
    (03.00, 12.00),
    (00.00, 07.00),
    (03.00, 12.00),
    (03.00, 00.00),
    (06.00, 00.00)
]
_2 = [
    ORIGIN,
    (06.00, 08.00),
    (05.00, 10.50),
    (04.00, 11.50),
    (03.00, 12.00),
    (02.00, 11.50),
    (01.00, 10.50),
    (00.00, 08.00),
    (01.00, 10.50),
    (02.00, 11.50),
    (03.00, 12.00),
    (04.00, 11.50),
    (05.00, 10.50),
    (06.00, 08.00),
    ORIGIN,
    (06.00, 00.00)
]
_3 = [
    (00.00, 01.00),
    (03.00, 00.00),
    (04.50, 01.00),
    (06.00, 03.00),
    (04.50, 05.00),
    (02.00, 06.00),
    (04.50, 07.00),
    (06.00, 09.00),
    (04.50, 11.00),
    (03.00, 12.00),
    (00.00, 11.00),
    (03.00, 12.00),
    (04.50, 11.00),
    (06.00, 09.00),
    (04.50, 07.00),
    (02.00, 06.00),
    (04.50, 05.00),
    (06.00, 03.00),
    (04.50, 01.00)
]
_4 = [
    (04.00, 00.00),
    (04.00, 12.00),
    (00.00, 05.00),
    (06.00, 05.00),
    (00.00, 05.00),
    (04.00, 12.00),
    (04.00, 00.00)
]
_5 = [
    ORIGIN,
    (00.00, 00.00),
    (02.00, 00.50),
    (04.00, 01.00),
    (05.00, 02.00),
    (06.00, 03.50),
    (05.00, 05.00),
    (04.00, 06.00),
    (02.00, 06.50),
    (00.00, 07.00),
    (00.00, 12.00),
    (06.00, 12.00),
    (00.00, 12.00),
    (00.00, 07.00),
    (02.00, 06.50),
    (04.00, 06.00),
    (05.00, 05.00),
    (06.00, 03.50),
    (05.00, 02.00)
]
_6 = [
    (03.00, 00.00),
    (01.00, 01.00),
    (00.00, 03.00),
    (01.00, 05.00),
    (02.00, 06.00),
    (06.00, 12.00),
    (02.00, 06.00),
    (05.00, 05.00),
    (06.00, 03.00),
    (05.00, 01.00),
    (03.00, 00.00),
]
_7 = [
    (02.00, 00.00),
    (04.00, 06.00),
    (02.00, 06.00),
    (04.00, 06.00),
    (06.00, 12.00),
    (00.00, 12.00),
    (06.00, 12.00),
    (02.00, 00.00),
]
_8 = [
    (03.00, 00.00),
    (01.00, 01.00),
    (00.00, 03.00),
    (01.00, 05.00),
    (03.00, 06.00),
    (01.00, 07.00),
    (00.00, 09.00),
    (01.00, 11.00),
    (03.00, 12.00),
    (05.00, 11.00),
    (06.00, 09.00),
    (05.00, 07.00),
    (03.00, 06.00),
    (05.00, 05.00),
    (06.00, 03.00),
    (05.00, 01.00),
    (03.00, 00.00)
]
_9 = [
    ORIGIN,
    (06.00, 00.00),
    (06.00, 09.00),
    (05.00, 11.00),
    (03.00, 12.00),
    (01.00, 11.00),
    (00.00, 09.00),
    (01.00, 07.00),
    (03.00, 06.00),
    (05.00, 07.00),
    (06.00, 09.00),
    (06.00, 00.00),
]
_ob = [
    (03.00, 01.00),
    (03.00, 03.00),
    (02.00, 05.00),
    (01.00, 06.00),
    (00.00, 06.00),
    (01.00, 06.00),
    (02.00, 07.00),
    (03.00, 09.00),
    (03.00, 11.00),
    (06.00, 12.00),
    (03.00, 11.00),
    (03.00, 09.00),
    (02.00, 07.00),
    (01.00, 06.00),
    (02.00, 05.00),
    (03.00, 03.00),
    (03.00, 01.00),
    (06.00, 00.00)
]
_cb = [
    ORIGIN,
    (03.00, 01.00),
    (03.00, 03.00),
    (04.00, 05.00),
    (05.00, 06.00),
    (06.00, 06.00),
    (05.00, 06.00),
    (04.00, 07.00),
    (03.00, 09.00),
    (03.00, 11.00),
    (00.00, 12.00),
    (03.00, 11.00),
    (03.00, 09.00),
    (04.00, 07.00),
    (05.00, 06.00),
    (04.00, 05.00),
    (03.00, 03.00),
    (03.00, 01.00)
]

CHRMAP = {
    'A': Char(_A),
    'B': Char(_B),
    'C': Char(_C),
    'D': Char(_D),
    'E': Char(_E),
    'F': Char(_F),
    'I': Char(_I),
    'N': Char(_N),
    'S': Char(_S),
    '0': Char(_0),
    '1': Char(_1),
    '2': Char(_2),
    '3': Char(_3),
    '4': Char(_4),
    '5': Char(_5),
    '6': Char(_6),
    '7': Char(_7),
    '8': Char(_8),
    '9': Char(_9),
    '{': Char(_ob),
    '}': Char(_cb)
}
