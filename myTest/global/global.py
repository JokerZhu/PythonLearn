#!/usr/bin/python3.4


x = 50
def func():
	global x
	print('x的值是', x)
	x = 2
	print('全局变量x改为', x)
func()
print('x的值是', x)
