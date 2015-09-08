#!/usr/bin/python3.4
import time
import sys
for i in range(5):
#	print('\r' ,i)
	sys.write(1,bytes(format(i,:qw)))

	sys.stdout.flush()
	time.sleep(1)
	
