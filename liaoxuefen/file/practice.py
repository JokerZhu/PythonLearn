#!/usr/bin/python3.4
#-*- coding = utf-8 -*-

#1、利用os模块编写一个能实现dir -l输出的程序。

import os
import os.path
'''
def cmdll(path):
	return os.listdir(path)




print(cmdll('/home/zhujingwei/git/PythonLearn'))
'''
#2、编写一个程序，能在当前目录以及当前目录的所有子目录下查找文件名包含指定字符串的文件，并打印出相对路径
'''
def find_name(path = '.', name = '.py'):
	for x in os.listdir(path):
		a = os.path.join(path,x)
		if(os.path.isfile(a)):
			if(os.path.splitext(a)[1] == name):
				print(a)
		else:
			find_name(a)

path = str(input('please input: ') )
print('path = ',path)
if os.listdir(path):
	find_name(path,'.py')
else:
	print('your input is not a dir')
'''
#3、编写一个程序，能在当前目录以及当前目录的所有子目录下查找文件包含指定字符串的文件，并打印出相对路径
'''
def cmdlR(path,string):
	for x in os.listdir(path):
		if os.path.isfile(os.path.join(path,x)):
			with open(os.path.join(path,x),'r') as f:
				if(f.readline().find(string) >= 0 ):
					print(os.path.join(path,x))
		if os.path.isdir(os.path.join(path,x)):
			print('')

cmdlR('/home/zhujingwei/git/PythonLearn','earn')
'''
