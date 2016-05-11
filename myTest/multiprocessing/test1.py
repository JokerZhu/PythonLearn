#!/usr/bin/python3.4
import multiprocessing
import time
import os

def func(msg):
	for i in range(3):
		print( msg)
		print( os.getpid())
		time.sleep(1)

if __name__ == "__main__":
	p = multiprocessing.Process(target=func, args=("hello", ))
	p.start()
	p.join()
	print ("Sub-process done.")
