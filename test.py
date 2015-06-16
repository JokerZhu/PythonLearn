#!/usr/local/bin/python3.4
# -*- coding: UTF-8 -*-
import math

#a = input("please input a str:") 
#print('you have inputed str = ',a)
#b = '朱静玮'
#print('b = ',b)
'''
a = input("please input your name:") 
b = 'hello %s' % a
print('b = ',b)
'''
'''

def move(x, y, step, angle=0):
    nx = x + step * math.cos(angle)
    ny = y - step * math.sin(angle)
    return nx, ny
r = move(1,2,3)
#print(move(1,2,3))
print(r)
'''
'''
def quadratic(a,b,c ):
	x1=(-b+math.sqrt(b*b-4*a*c))/2*b
	x2=(-b-math.sqrt(b*b-4*a*c))/2*b
	return x1,x2

print(quadratic(2,3,1) )
'''
'''
def fact(n):
	if not isinstance(n,(int,float)):
		raise TypeError('bad operand type')
	if n == 1 or n == 0:
		return n
	else:
		return n * fact(n - 1)	

print(fact(4))
'''
g = [x * x for x in range(10) ]
print(g)
g = (x * x for x in range(10) )
for n in g:
	print(n)
