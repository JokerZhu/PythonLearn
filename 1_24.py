#!/usr/bin/python3.4
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

'''
g = [x * x for x in range(10) ]
print(g)
g = (x * x for x in range(10) )
for n in g:
	print(n)
'''

'''
def fun1():
	x = 5
	def fun2():
		nonlocal x
		x *= x
		return x
	return(fun2())

print(fun1())
'''
'''
#lambda
g = lambda x,y : 2 * x +y
print(g(5,7))
'''
#fliter


'''
def myFun(x):
	if x == 1:
		return x
	else:
		return x * myFun(x - 1)

def myFun2(x):
	result = x
	for i in range(1,x):
		result *= i
	return result

#print(myFun2(10)) 
print(myFun(5)) 
'''
'''
def FBOLAQI(x):
	n1 = 1 
	n2 = 1
	n3 = 1
	if x < 1:
		print('input err :',x)
		return -1
	while x > 2:
		n3 = n1 + n2
		n1 = n2
		n2 = n3
		x -= 1
	return n3

def FBOLAQI2(x):
	if x < 1:
		print('input err :',x)
		return -1
	if x == 2 or x == 1:
		return 1
	else:
		return FBOLAQI2(x - 1) + FBOLAQI2(x -2)
		

print(FBOLAQI(35))
print(FBOLAQI2(35))
'''
#汉诺塔
def hanoi(n,x,y,z):
	if n == 1:
		print(x,'-->',z)
	else:
		#将前n-1个盘子移动到y上
		hanoi(n-1,x,z,y) #将y上面的n-1个盘子移动到z上
		print(x,'-->',z) #将最后一个盘子从x移动到z上
		hanoi(n-1,y,x,z) #将y上面的n-1个盘子移动到z上
n = int(input('请输入汉诺塔的层数：'))
hanoi(n,'x','y','z')
