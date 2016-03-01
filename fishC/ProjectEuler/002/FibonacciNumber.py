#!/usr/bin/python3.4

def GetFibonacciList(num = 0):
	if num == 0:
		return 0
	elif num == 1:
		return 1
	else:
		return  GetFibonacciList(num -1) +  GetFibonacciList(num - 2)

def flb3(n):
	a,b = 0,1
	for i in range(n):
		a,b = b, a + b
	return a


if __name__ == '__main__':
	'''
	sum = 0
	for i in range(0,30):
		print(i)
		a = GetFibonacciList(i)
		if a % 2 == 0:
			sum += a
	print('sum = ',sum)
	'''
	SUM = 0
	l1 = [1,2]
	while(True):
		n = l1[-1]+l1[-2]
		if n > 4000000:
			break
		else:
			l1.append(n)
	for n in l1:
		if n % 2 == 0:
			SUM += n
	print(SUM)
	#print(flb3(10))
