#!/usr/bin/env python3

import os
import subprocess

from itertools import combinations

import gen_cpuseq

iters = 1000000


def sh(*args):
    print(args)
    out = subprocess.check_output(args)
    return out.decode("utf8").strip()
#print(out.strip())
#return out.strip()


def get_cores():
    """ debug
    for cpu in gen_cpuseq.seq(gen_cpuseq.cpuinfo):
        print(cpu)
        print()
    """
    return [int(cpu["processor"]) for cpu in gen_cpuseq.seq(gen_cpuseq.cpuinfo)]


def __get_values(a, b):
    tok = sh("./o/reftable", str(a), str(b), str(iters)).split(" ")
    tok = tok[0].split('\n')
    tok = map(lambda x: int(x), tok)
    tok_min = 9999999

    f = open('output/%d-%d.txt' % (a, b), 'w')
    for i in tok:
        f.write("%d\n" % i)
        tok_min = min(tok_min, i)
    f.close()
    return tok_min
    #return min(tok)


def build_table(a, b):
    return min(__get_values(a, b), __get_values(b, a))

def __ensure_output_dir():
    output_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'output')
    if os.path.isdir(output_path):
        return
    os.makedirs(output_path)

max_offset = 0
__ensure_output_dir()
for (a, b) in combinations(get_cores(), 2):
    print("(a, b)", a, b)
    key = (min(a, b), max(a, b))
    max_offset = max(build_table(*key), max_offset)
    print ("(%d, %d) ... done" % (a, b))
print (max_offset)
