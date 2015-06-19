#!/usr/bin/python3.4
# -*- coding: utf-8 -*-

try:
	print('try...\n')
	r = 10 / 2
	print('result:',r)
except ZeroDivisionError as e:
	print('except:',e)
finally:
	print('finally...')
print('end')
