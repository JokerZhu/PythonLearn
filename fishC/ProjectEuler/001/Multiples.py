#!/usr/bin/python3.4
def GetMutipls(num = 0):
	sum = 0
	for i in range(0,num):
		if i%3 == 0 or i%5 == 0:
			sum += i
	return sum

if __name__ == '__main__':
	num = 1000
	print(GetMutipls(num))
