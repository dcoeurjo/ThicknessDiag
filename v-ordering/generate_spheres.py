import os
import sys
import argparse
import glob
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
    """Validate user input positive float."""
    try:
        flt = float(flt)
        if flt <= 0:
            raise argparse.ArgumentError('Must be positive float')
        return flt
    except ValueError:
        raise argparse.ArgumentError('Must be floating point number')

def positive_int(nb):
    """Validate user input positive integer."""
    try:
        nb = int(nb)
        if nb <= 0:
            raise argparse.ArgumentError('Must be positive integer')
        return nb
    except ValueError:
        raise argparse.ArgumentError('Must be an integer')

# argument defaults
defaults = {
        'format': '{ %s, %s, %s, %s }',
        'separator': ',\n',
        'number': 100,
        'dir': 'test_spheres',
        }

# argument parser
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

# main program
if __name__ == '__main__':
    parsed_args = parser.parse_args()
    gen = partial(generate_sphere, coord_amp=parsed_args.coord_amp,
            radius_amp=parsed_args.radius_amp)
    spheres = [parsed_args.format % tuple(gen()) for _ in xrange(parsed_args.number)]
    spheres_data = parsed_args.separator.join(spheres) + '\n'
    if parsed_args.stdout:
        sys.stdout.write(spheres_data)
    else:
        this_dir = os.path.dirname(os.path.realpath(__file__))
        files_dir = os.path.join(this_dir, defaults['dir'])
        files = map(os.path.basename, glob.glob(os.path.join(files_dir, '*.h')))
        if not files:
            file_index = '0'
        else:
            files_range = sorted(map(lambda x: int(x.split('.')[0]), files))
            file_index = files_range[-1] + 1
        with open(os.path.join(files_dir, str(file_index) + '.h'), 'w') as file_:
            file_.write(spheres_data)
