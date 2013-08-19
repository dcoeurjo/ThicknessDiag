import sys
import argparse
from functools import partial
from random import random

def string_format(fmt):
    """Validate user input format (by try-error)."""
    try:
        fmt % (0, 0, 0, 0)
    except TypeError:
        raise argparse.ArgumentError('Bad format string')
    return fmt

def positive_float(flt):
    try:
        flt = float(flt)
        if flt <= 0:
            raise argparse.ArgumentError('Must be positive float')
        return flt
    except ValueError:
        raise argparse.ArgumentError('Must be floating point number')

def positive_int(nb):
    try:
        nb = int(nb)
        if nb <= 0:
            raise argparse.ArgumentError('Must be positive integer')
        return nb
    except ValueError:
        raise argparse.ArgumentError('Must be an integer')

defaults = {
        'format': '{ %s, %s, %s, %s }',
        'separator': ',\n',
        'number': 100,
        'file': 'test_spheres.h'
        }

description = 'Generate spheres in a given format (default=%s)' % defaults['format']
parser = argparse.ArgumentParser(description=description)
parser.add_argument('coord_amp', metavar='COORD_AMPLITUDE', type=positive_float)
parser.add_argument('radius_amp', metavar='RADIUS_AMPLITUDE', type=positive_float)
parser.add_argument('--with-format', dest='format',
        help='Format for output (keys are: x, y, z, radius)',
        type=string_format, default=defaults['format'])
parser.add_argument('--with-separator', dest='separator',
        help='Separator between spheres generated',
        default=defaults['separator'])
parser.add_argument('-n', '--number', help='Number of spheres to generate',
        type=positive_int, default=defaults['number'])
parser.add_argument('--with-stdout', dest='stdout', help='Print output to STDOUT',
        action='store_true', default=False)

def generate_sphere(coord_amp, radius_amp):
    """Generate a dictionary representing a sphere."""
    return map(str, [random()*coord_amp, random()*coord_amp,
        random()*coord_amp, random()*radius_amp])

if __name__ == '__main__':
    parsed_args = parser.parse_args()
    gen = partial(generate_sphere, coord_amp=parsed_args.coord_amp,
            radius_amp=parsed_args.radius_amp)
    spheres = [parsed_args.format % tuple(gen()) for _ in xrange(parsed_args.number)]
    spheres_data = parsed_args.separator.join(spheres) + '\n'
    if parsed_args.stdout:
        sys.stdout.write(spheres_data)
    else:
        with open(defaults['file'], 'w') as file_:
                file_.write(spheres_data)
