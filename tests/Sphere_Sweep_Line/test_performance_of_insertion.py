#!/usr/bin/env python

import os
import subprocess as sp
import multiprocessing as mp

# this directory
this_dir = os.path.dirname(os.path.realpath(__file__))

# name of the executable
exe_name = 'Sphere_Sweep_Line'
# ...generated full path
exe_filename = os.path.join(this_dir, 'build', exe_name)
# ...check this one
if not os.path.exists(exe_filename):
    print 'File', exe_filename, 'does not exist'
# ...helper for running and capturing each output
def run_executable(*args):
    p = sp.Popen([exe_filename] + map(str, args),
            stdout=sp.PIPE, stderr=sp.STDOUT)
    while True:
        retcode = p.poll()
        for line in p.stdout:
            line = line.strip()
            if line:
                yield line
        if retcode is not None:
            break

# name of the output CSV file
out_filename = 'timing.csv'
out_file = open(out_filename, 'w')
out_lock = mp.Lock()
#...helper for writing
def write_to_csv(tc, tt):
    # takes test case and total time
    out_lock.acquire()
    msg = '%s;%s;%s\n' % (tc[0], tc[1], tt)
    out_file.write(msg)
    out_file.flush()
    out_lock.release()

# test inputs
test_nb_spheres = (10, 20, 30, 40, 50, 75, 100, 125, 150, 200, 250, 300)
test_rand_amps = (10, 100, 1000, 10000, 100000)

# test cases
def test_cases():
    for nb_spheres in test_nb_spheres:
        for rand_amp in test_rand_amps:
            yield nb_spheres, rand_amp
# handle a test case (multiprocess)
def handle_test_case(tc):
    assert tc[0] != 0
    tt = sum(map(lambda s: float(s.split()[0]),
        run_executable(*tc))) / tc[0]
    write_to_csv(tc, tt)

if __name__ == '__main__':
    pool = mp.Pool()
    try:
        pool.map(handle_test_case, test_cases())
    except KeyboardInterrupt:
        pool.terminate()
