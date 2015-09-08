#!/usr/bin/python3.4
import os,sys,string   
import time

def view_bar(num=1, sum=100, bar_word='='):
	rate = float(num) / float(sum)
	rate_num = int(rate * 100)
#	os.write(1,bytes('\r{}%:'.format(rate_num),'utf-8'))
	for i in range(0, num):
		os.write(1,bytes(bar_word,'utf-8'))
		#sys.stdout.flush()
	os.write(1,bytes('\r{}%:'.format(rate_num),'utf-8'))
	sys.stdout.flush()

def my_view_bar(num=1,sum=100,bar_word='='):
	rate = float(num) / float(sum)
	rate_num = int(rate * 100)
	os.write(1,bytes('{}%'.format(rate_num),'utf-8'))
	for i in range(0,num):
		os.write(1,bytes(bar_word,'utf-8'))
	sys.stdout.flush()
	
def progress_test():
    bar_length=100
    for percent in range(0, 100):
        hashes = '#' * int(percent/100.0 * bar_length)
        spaces = ' ' * (bar_length - len(hashes))
        sys.stdout.write("\rPercent: [%s] %d%%"%(hashes + spaces, percent))
        sys.stdout.flush()
        time.sleep(1)
 

if __name__ == '__main__':
	'''
	for i in range(0, 100):
		time.sleep(0.1)
		#view_bar(i, 100,'=')
		my_view_bar(i, 100,'=')
	'''

	progress_test()

