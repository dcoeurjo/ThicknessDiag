import sys
import math
import functools

"""
Filtering script, currently used to debug the rotation
bug in CircleView's fromCircle static builder method.
"""

# the identity matrix
identity_matrix = ((1, 0, 0), (0, 1, 0), (0, 0, 1))

def dot_product(u, v):
    # Compute the dot product between the two vectors u.v
    return reduce(lambda s, x: s + x[0]*x[1], zip(u, v), 0)

def matrix_mult(matrix, v):
    # Multiply the matrix by v as M*v
    return tuple(dot_product(v, l) for l in matrix)

def rotate_around_x(vec, angle):
    # Apply a rotation of "angle" around the x-axis
    return matrix_mult((
        (1, 0, 0),
        (0, math.cos(angle), -math.sin(angle)),
        (0, math.sin(angle), math.cos(angle)),
        ), vec)

def rotate_around_y(vec, angle):
    # Apply a rotation of "angle" around the y-axis
    return matrix_mult((
        (math.cos(angle), 0, math.sin(angle)),
        (0, 1, 0),
        (-math.sin(angle), 0, math.cos(angle))
        ), vec)

def rotate_around_z(vec, angle):
    # Apply a rotation of "angle" around the z-axis
    return matrix_mult((
        (math.cos(angle), -math.sin(angle), 0),
        (math.sin(angle), math.cos(angle), 0),
        (0, 0, 1)
        ), vec)

def rotate(vec, angles):
    # Combined version of all the previous
    return reduce(lambda v, r: r(v), (
        functools.partial(rotate_around_z, angle=angles[2]),
        functools.partial(rotate_around_y, angle=angles[1]),
        functools.partial(rotate_around_x, angle=angles[0]),
        ), vec)

def read_data(fp):
    # Generate the lines as a list of floats
    for line in fp.readlines():
        line = line.strip()
        if line and not line.startswith('#'):
            yield map(float, line.split()[:6])
        else:
            print line

# Data format: [x-rot, y-rot, z-rot, vx, vy, vz]
for d in read_data(sys.stdin):
    angles = map(lambda x: -x*math.pi/180., d[:3])
    vec = d[3:]
    rotated_vec = rotate((0, 0, 1), angles)
    epsilon = 0.01
    valid = all(abs(x - y) < epsilon for x, y in zip(vec, rotated_vec))
    print ' '.join(map(str, map(lambda x: -x*180./math.pi, angles) + vec)), '->', valid
