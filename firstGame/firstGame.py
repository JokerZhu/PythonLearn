#!/usr/bin/python3.4
import random
secret = random.randint(1,10)
temp = input('猜猜俺现在心里在想啥数字？请输入：') 
no = int(temp)
i = 0
j = 3
print('secret = ',secret)
while no != secret and i < j:
	if(no > secret):
		print('大啦大啦')
	else:
		print('小啦小啦')
	if i < j-1:
		temp = input('错啦？请重新输入：') 
		no = int(temp)
	else:
		print('机会用完啦，再见')
	i=i+1
if no == secret:
	print('你牛逼，猜中了\n')
