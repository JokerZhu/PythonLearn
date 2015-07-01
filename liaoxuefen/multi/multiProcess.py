#!/usr/bin/python3.4
# -*- coding = utf-8 -*-

import os,subprocess,time,random
from multiprocessing import Process,Queue
'''
print('now the proce %d is running' % os.getpid() )
if os.fork():
	print('now the father proce %d is running' % os.getpid() )
else:
	print('now the child proce %d is running' % os.getpid() )

'''
'''
print('$nslookup www.python.org')
f  = subprocess.call(['nslookup','www.python.org'])
print('Exit code :',f)
'''

def write(q):
	print('Pricess to wirite : %d' % os.getpid())
	for value in ['A','B','C']:
		print('Put %s to queue..' % value)
		q.put(value)
		time.sleep(random.random())


def read(q):
	print('Process to read : %d ' % os.getpid())
	while True:
		value = q.get(True)
		print('Get %s form queue.' % value)


if __name__ == '__main__':
	q = Queue()
	pw = Process(target = write,args = (q,))
	pr = Process(target = read, args = (q,))

	pw.start()
	pr.start()
	pw.join()
	pr.terminate()
