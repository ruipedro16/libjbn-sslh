from sage.all import *

# NIST P-384 CURVE
p = 2**384 - 2**128 - 2**96 + 2**32 - 1
R = IntegerModRing(p)
Fp = FiniteField(p)
A = -3
B = 0xB3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF

E = EllipticCurve(Fp, [A, B])

Gx = 0xAA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7
Gy = 0x3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F
assert E.is_on_curve(Gx, Gy)
G = E([Gx, Gy])

POINT_INIFINITY = E(0)  # (0 : 1 : 0)

# works


def ladder_sage(k, P):
    """
    [https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication#Montgomery_ladder]

    R0 ← 0
    R1 ← P
    for i from m downto 0 do
        if di = 0 then
            R1 ← point_add(R0, R1)
            R0 ← point_double(R0)
        else
            R0 ← point_add(R0, R1)
            R1 ← point_double(R1)
    return R0
    """
    R0 = POINT_INIFINITY
    R1 = P

    k_bin = bin(k)[2:]  # binary representation of k
    k_bin_reversed = k_bin[::-1]  # reversed binary representation of k
    k_len = len(k_bin)  # number of bits in k

    for i in range(k_len):
        if di := k_bin_reversed[i] == "0":
            R1 = R0 + R1
            R0 = 2 * R0
        else:
            R0 = R0 + R1
            R1 = 2 * R1

    return R0


def scalar_mult(Px, Py, Pz, k, b=B):
    """
    [https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication#Montgomery_ladder]

    R0 ← 0
    R1 ← P
    for i from m downto 0 do
        if di = 0 then
            R1 ← point_add(R0, R1)
            R0 ← point_double(R0)
        else
            R0 ← point_add(R0, R1)
            R1 ← point_double(R1)
    return R0
    """
    R0x, R0y, R0z = 0, 1, 0
    R1x, R1y, R1z = Px, Py, Pz

    k_bin = bin(k)[2:]  # binary representation of k
    k_bin_reversed = k_bin[::-1]  # reversed binary representation of k
    k_len = len(k_bin)  # number of bits in k

    for i in range(k_len):
        if di := k_bin_reversed[i] == "0":
            R1x, R1y, R1z = projective_add(R0x, R0y, R0z, R1x, R1y, R1z)  # R0 + R1
            R0x, R0y, R0z = point_doubling(R0x, R0y, R0z)  # 2 * R0
        else:
            R0x, R0y, R0z = projective_add(R0x, R0y, R0z, R1x, R1y, R1z)  # R0 + R1
            R1x, R1y, R1z = point_doubling(R1x, R1y, R1z)  # 2 * R1

    return R0x, R0y, R0z


def normalize(X, Y, Z):
    return (X / Z, Y / Z, 1)


def projective_add(X1, Y1, Z1, X2, Y2, Z2, b=B):
    """
    [Alg 4]: https://eprint.iacr.org/2015/1060.pdf
    Curve E: Y^2.Z = X^3 - 3XZ^2 + bZ^3
    Point P = (X1, Y1, Z1), Q = (X2, Y2, Z2)
          P + Q = (X3, Y3, Z3)
    """
    t0 = X1 * X2
    t1 = Y1 * Y2
    t2 = Z1 * Z2

    t3 = X1 + Y1
    t4 = X2 + Y2
    t3 = t3 * t4

    t4 = t0 + t1
    t3 = t3 - t4
    t4 = Y1 + Z1

    X3 = Y2 + Z2
    t4 = t4 * X3
    X3 = t1 + t2

    t4 = t4 - X3
    X3 = X1 + Z1
    Y3 = X2 + Z2

    X3 = X3 * Y3
    Y3 = t0 + t2
    Y3 = X3 - Y3

    Z3 = b * t2
    X3 = Y3 - Z3
    Z3 = X3 + X3

    X3 = X3 + Z3
    Z3 = t1 - X3
    X3 = t1 + X3

    Y3 = b * Y3
    t1 = t2 + t2
    t2 = t1 + t2

    Y3 = Y3 - t2
    Y3 = Y3 - t0
    t1 = Y3 + Y3

    Y3 = t1 + Y3
    t1 = t0 + t0
    t0 = t1 + t0

    t0 = t0 - t2
    t1 = t4 * Y3
    t2 = t0 * Y3

    Y3 = X3 * Z3
    Y3 = Y3 + t2
    X3 = t3 * X3

    X3 = X3 - t1
    Z3 = t4 * Z3
    t1 = t3 * t0

    Z3 = Z3 + t1

    return X3, Y3, Z3


def mixed_add(X1, Y1, Z1, X2, Y2, b=B):
    """
    [Alg 5]: https://eprint.iacr.org/2015/1060.pdf
    Curve E: Y^2.Z = X^3 - 3XZ^2 + bZ^3
    Point P = (X1, Y1, Z1), Q = (X2, Y2, 1)
          P + Q = (X3, Y3, Z3)
    """
    t0 = X1 * X2
    t1 = Y1 * Y2
    t3 = X2 + Y2

    t4 = X1 + Y1
    t3 = t3 * t4
    t4 = t0 + t1

    t3 = t3 - t4
    t4 = Y2 * Z1
    t4 = t4 + Y1

    Y3 = X2 * Z1
    Y3 = Y3 + X1
    Z3 = b * Z1

    X3 = Y3 - Z3
    Z3 = X3 + X3
    X3 = X3 + Z3

    Z3 = t1 - X3
    X3 = t1 + X3
    Y3 = b * Y3

    t1 = Z1 + Z1
    t2 = t1 + Z1
    Y3 = Y3 - t2

    Y3 = Y3 - t0
    t1 = Y3 + Y3
    Y3 = t1 + Y3

    t1 = t0 + t0
    t0 = t1 + t0
    t0 = t0 - t2

    t1 = t4 * Y3
    t2 = t0 * Y3
    Y3 = X3 * Z3

    Y3 = Y3 + t2
    X3 = t3 * X3
    X3 = X3 - t1

    Z3 = t4 * Z3
    t1 = t3 * t0
    Z3 = Z3 + t1

    return X3, Y3, Z3


def point_doubling(X, Y, Z, b=B):
    """
    [Alg 6]: https://eprint.iacr.org/2015/1060.pdf
    Curve E: Y^2.Z = X^3 - 3XZ^2 + bZ^3
    Point P = (X, Y, Z)
    """
    t0 = X * X
    t1 = Y * Y
    t2 = Z * Z

    t3 = X * Y
    t3 = t3 + t3
    Z3 = X * Z

    Z3 = Z3 + Z3
    Y3 = b * t2
    Y3 = Y3 - Z3

    X3 = Y3 + Y3
    Y3 = X3 + Y3
    X3 = t1 - Y3

    Y3 = t1 + Y3
    Y3 = X3 * Y3
    X3 = X3 * t3

    t3 = t2 + t2
    t2 = t2 + t3
    Z3 = b * Z3

    Z3 = Z3 - t2
    Z3 = Z3 - t0
    t3 = Z3 + Z3

    Z3 = Z3 + t3
    t3 = t0 + t0
    t0 = t3 + t0

    t0 = t0 - t2
    t0 = t0 * Z3
    Y3 = Y3 + t0

    t0 = Y * Z
    t0 = t0 + t0
    Z3 = t0 * Z3

    X3 = X3 - Z3
    Z3 = t0 * t1
    Z3 = Z3 + Z3

    Z3 = Z3 + Z3

    return X3, Y3, Z3


"""
Test doubling

xG, yG, zG = G
x, y, z = 2 * G
_x, _y, _z = point_doubling(R(xG), R(yG), R(zG))
_x, _y, _z = normalize(_x, _y, _z)

assert x == _x
assert y == _y
assert z == _z
"""

"""
Test mixed addition

xG, yG, zG = G
x2G, y2G, _ = 2 * G # (X : Y : 1)
_x2G, _y2G, _z2G = point_doubling(R(xG), R(yG), R(zG)) # (X : Y : Z)
x, y, z = 4 * G 

_x, _y, _z = mixed_add(_x2G, _y2G, _z2G, x2G, y2G) # 2G + 2G = 4G
_x, _y, _z = normalize(_x, _y, _z)

assert x == _x
assert y == _y
assert z == _z
"""

"""
Test projective addition

xG, yG, zG = G
x, y, z = 4 * G 
_x2G, _y2G, _z2G = point_doubling(R(xG), R(yG), R(zG)) # (X : Y : Z)
_x, _y, _z = projective_add(_x2G, _y2G, _z2G, _x2G, _y2G, _z2G)
_x, _y, _z = normalize(_x, _y, _z)

assert x == _x
assert y == _y
assert z == _z
"""

"""
Test scalar multiplication

xG, yG, zG = 7 * G
print(xG)
print(yG)
print(zG)

print("\n")

# r = ladder_sage(7, G)
# rx, ry, rz = r

# print(rx)
# print(ry)
# print(rz)

rx, ry, rz = scalar_mult(R(G[0]), R(G[1]), R(1), 7)
rx, ry, rz = normalize(rx, ry, rz)
print(rx)
print(ry)
print(rz)
"""
