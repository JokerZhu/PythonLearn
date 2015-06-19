#!/usr/bin/python3.4
# -*- coding:utf-8 -*-
import logging
logging.basicConfig(level=logging.WARNING)

'''
def log(func):
	def wrapper(*args,**kw):
		print('call %s():' % func.__name__)
		return func(*args,**kw)
	return wrapper
@log
def now():
	print('2015-06-16')

now()
'''
'''
def foo(s):
	n = int(s)
#	assert n != 0, 'n is zero!'
	return 10 / n

n = 10
logging.info('n = %s' % n)
foo('0')
'''



def abs(n):
	'''
	Example:
	>>>abs(1)
	1
	>>>abs(-1)
	1
	>>>abs(0)
	0
	'''
	return n if n >= 0 else (-n)
