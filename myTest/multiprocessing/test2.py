#!/usr/bin/python3.4
from multiprocessing import Pool
import time
import os


def f(x):
	print( x*x)
	print( os.getpid())


if __name__ == '__main__':
	pool = Pool(processes=4)              # start 4 worker processes
	result = pool.apply_async(f, (10,))    # evaluate "f(10)" asynchronously
	result.get(timeout=1)           # prints "100" unless your computer is *very* slow
	pool.map(f, range(10))          # prints "[0, 1, 4,..., 81]"

	it = pool.imap(f, range(10))
	it.next()                       # prints "0"
	it.next()                       # prints "1"
	it.next(timeout=1)              # prints "4" unless your computer is *very* slow
'''
	result = pool.apply_async(time.sleep, (10,))
	print (result.get(timeout=1))           # raises TimeoutError
'''
