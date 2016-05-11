#!/usr/bin/python3.4
import multiprocessing
import time
import os
 
def func(msg):
	for i in range(3):
		print (msg)
		print (os.getpid())
		print("-" * 20)
		time.sleep(1)
 
if __name__ == "__main__":
	pool = multiprocessing.Pool(processes=4)
	for i in range(10):
		msg = "hello %d" %(i)
		pool.apply_async(func, (msg, ))
	pool.close()
	pool.join()
	print ("Sub-process(es) done.")
